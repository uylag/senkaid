#pragma once

// utils.hpp: Basic memory utilities for the senkaid library.
// Provides low-level memory operations like zeroing, copying, and alignment checks.
// Uses macros from compiler.hpp for compiler detection and support.

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>

// Conditional includes for portability
#include "utils/config/root.hpp"
#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint> // For std::size_t, std::uintptr_t
    #endif
    #if SENKAID_HAS_INCLUDE(<cstring>)
        #include <cstring> // For std::memset, std::memcpy
    #endif
    #if defined(SENKAID_COMPILER_NVCC) || defined(__CUDACC__)
        #if SENKAID_HAS_INCLUDE(<cuda_runtime.h>)
            #include <cuda_runtime.h> // For CUDA memory functions
        #endif
    #endif
    #if SENKAID_HAS_INCLUDE(<memory>)
        #include <memory>
    #endif
#else
    #include <cstdint>
    #include <cstring>
    #if defined(SENKAID_COMPILER_NVCC) || SENKAID_HAS_INCLUDE(<cuda_runtime.h>) || defined(__CUDACC__)
        #include <cuda_runtime.h>
    #endif
    #include <memory>
#endif

namespace senkaid::memory {

SENKAID_DIAGNOSTIC_PUSH
SENKAID_IGNORE_WARNING_UNUSED_FUNCTION

template <typename TN>
concept NumericType = std::is_arithmetic_v<TN>;

// --- Core Host Memory Operations ---

// zero_memory: Fills a block of host memory with zeros.
// Parameters:
//   ptr  - Pointer to the host memory block to fill. No-op if nullptr.
//   size - Number of bytes to fill.
SENKAID_FORCE_INLINE void zero_memory(void* ptr, std::size_t size) {
    if (SENKAID_UNLIKELY(ptr == nullptr)) {
        SENKAID_LOG_WARNING("zero_memory: null pointer");
        return;
    }
    std::memset(ptr, 0, size);
}

// copy_memory: Copies a block of host memory from source to destination.
// Parameters:
//   dst  - Pointer to the destination host memory block. No-op if nullptr.
//   src  - Pointer to the source host memory block. No-op if nullptr.
//   size - Number of bytes to copy.
SENKAID_FORCE_INLINE void copy_memory(void* dst, const void* src, std::size_t size) {
    if (SENKAID_UNLIKELY(dst == nullptr || src == nullptr)) {
        SENKAID_LOG_WARNING("copy_memory: null pointer");
        return;
    }
    std::memcpy(dst, src, size);
}

// fill_memory: Fills a block of host memory with a specific byte value.
// Parameters:
//   ptr   - Pointer to the host memory block to fill. No-op if nullptr.
//   size  - Number of bytes to fill.
//   value - The byte value to fill the memory with.
SENKAID_FORCE_INLINE void fill_memory(void* ptr, std::size_t size, std::uint8_t value) {
    if (SENKAID_UNLIKELY(ptr == nullptr)) {
        SENKAID_LOG_WARNING("fill_memory: null pointer");
        return;
    }
    std::memset(ptr, static_cast<int>(value), size);
}

// --- CUDA-Specific Memory Operations ---
// Note: These functions assume the pointers refer to device memory when called on the host
//       via the CUDA Runtime API. For actual device code (__device__), simple loops
//       or shared memory operations are more common.

#if defined(SENKAID_COMPILER_NVCC) || defined(__CUDACC__)

    // zero_cuda_memory: Fills a block of device memory with zeros.
    // Template version for generic type support.
    // This function is intended to be called from host code.
    template<NumericType T>
    SENKAID_FORCE_INLINE void zero_cuda_memory(T* ptr, std::size_t size, cudaStream_t stream = nullptr) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            SENKAID_LOG_WARNING("zero_cuda_memory: null pointer");
            return;
        }
        auto result = stream ? cudaMemsetAsync(ptr, 0, size * sizeof(T), stream)
                             : cudaMemset(ptr, 0, size * sizeof(T));
        if (SENKAID_DEBUG_ONLY(result != cudaSuccess)) {
            SENKAID_LOG_ERROR("zero_cuda_memory: cudaMemset failed: " + std::string(cudaGetErrorString(result)));
        }
    } 

    // Overload for HostToDevice copy (Host source to Device destination)
    template<NumericType T>
    SENKAID_FORCE_INLINE void copy_cuda_memory(T* dst, const T* src, std::size_t size, cudaMemcpyKind kind = cudaMemcpyHostToDevice, cudaStream_t stream = nullptr) {
        if (SENKAID_UNLIKELY(dst == nullptr || src == nullptr)) {
            SENKAID_LOG_WARNING("copy_cuda_memory: null pointer");
            return;
        }
        auto result = stream ? cudaMemcpyAsync(dst, src, size * sizeof(T), kind, stream)
                             : cudaMemcpy(dst, src, size * sizeof(T), kind);
        if (SENKAID_DEBUG_ONLY(result != cudaSuccess)) {
            SENKAID_LOG_ERROR("copy_cuda_memory: cudaMemcpy failed: " + std::string(cudaGetErrorString(result)));
        }
    }

    // --- CUDA Device Code (Executed on GPU) ---
    // These are simple loops suitable for use within __device__ kernels.
    // They are conditionally compiled only when __CUDA_ARCH__ is defined.

    // zero_cuda_device: Fills a block of memory within a CUDA kernel.
    // This is a simple parallel loop. For complex kernels, consider cooperative groups.
    template<NumericType T>
    SENKAID_CUDA_HOST_DEVICE void zero_cuda_device(T* ptr, std::size_t size) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            SENKAID_LOG_WARNING("zero_cuda_device: null pointer");
            return;
        }
        #ifdef __CUDA_ARCH__
            // Simple strided loop for parallel execution on device
            for (std::size_t i = threadIdx.x + blockIdx.x * blockDim.x; i < size; i += blockDim.x * gridDim.x) {
                ptr[i] = T(0);
            } 
        #else
            // Fallback or assertion if called on host with device function name
            // In practice, host code should use zero_cuda_memory(T*)
            // This branch should ideally not be reached if used correctly.
            SENKAID_ASSERT(false, "zero_cuda_device called on host");
        #endif
    }

    // copy_cuda_device: Copies a block of memory within a CUDA kernel.
    // Note: Does not handle overlapping memory regions correctly.
    template<NumericType T>
    SENKAID_CUDA_HOST_DEVICE void copy_cuda_device(T* dst, const T* src, std::size_t size) {
        if (SENKAID_UNLIKELY(dst == nullptr || src == nullptr)) {
            SENKAID_LOG_WARNING("copy_cuda_device: dst or src is null pointer");
            return;
        }
        #ifdef __CUDA_ARCH__
            if (SENKAID_UNLIKELY(dst <= src && src < dst + size)) {
                SENKAID_ASSERT(false, "Overlapping device regions are not supported in copy_cuda_device");
                return; // Avoid undefined behavior for overlapping regions
            }
            for (std::size_t i = threadIdx.x + blockIdx.x * blockDim.x; i < size; i += blockDim.x * gridDim.x) {
                dst[i] = src[i];
            }
        #else
            SENKAID_ASSERT(false, "copy_cuda_device called on host");
        #endif
    }


#else // !defined(SENKAID_COMPILER_NVCC)

    // No-op stubs or fallbacks for non-CUDA builds
    // These mimic the host-side CUDA API behavior for host memory.

   template<NumericType T>
    SENKAID_FORCE_INLINE void zero_cuda_memory(T* ptr, std::size_t size, void* = nullptr) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            SENKAID_LOG_WARNING("zero_cuda_memory: null pointer");
            return;
        }
        std::memset(ptr, 0, size * sizeof(T));
    }

    template<NumericType T>
    SENKAID_FORCE_INLINE void copy_cuda_memory(T* dst, const T* src, std::size_t size, int = 0, void* = nullptr) {
        if (SENKAID_UNLIKELY(dst == nullptr || src == nullptr)) {
            SENKAID_LOG_WARNING("copy_cuda_memory: null pointer");
            return;
        }
        std::memcpy(dst, src, size * sizeof(T));
    }

    template<NumericType T>
    SENKAID_FORCE_INLINE void copy_cuda_device(T* dst, const T* src, std::size_t size) {
        if (SENKAID_UNLIKELY(dst == nullptr || src == nullptr)) {
            SENKAID_LOG_WARNING("copy_cuda_device: null pointer");
            return;
        }
        std::memcpy(dst, src, size * sizeof(T));
    }

    template<NumericType T>
    void zero_cuda_device(T*, std::size_t) 
    {
        throw std::runtime_error("Called CUDA function without CUDA support.");
    }

#endif // defined(SENKAID_COMPILER_NVCC)

// --- Debug Utilities ---

// is_aligned: Checks if a pointer is aligned to a specific boundary.
// Parameters:
//   ptr       - The pointer to check.
//   alignment - The required alignment (must be a power of two).
// Returns:
//   true if the pointer is aligned, false otherwise.
#if SENKAID_DEBUG

    // is_aligned: Checks if a pointer is aligned to a specific boundary.
    // Parameters:
    //   ptr       - The pointer to check.
    //   alignment - The required alignment (must be a power of two).
    // Returns:
    //   true if the pointer is aligned, false otherwise.
    SENKAID_FORCE_INLINE bool is_aligned(const void* ptr, std::size_t alignment) { // Убран SENKAID_DEBUG_ONLY
        // SENKAID_DIAGNOSTIC_PUSH перемещен внутрь, если SENKAID_ASSERT его требует,
        // или оставьте здесь, если он нужен для подавления предупреждений в этой функции.
        // В данном случае, SENKAID_UNLIKELY может вызывать предупреждение.

        if (SENKAID_UNLIKELY(ptr == nullptr || alignment == 0 || (alignment & (alignment - 1)) != 0)) {
            SENKAID_LOG_WARNING("is_aligned: invalid input");
            // Убедитесь, что SENKAID_ASSERT не использует свой собственный PUSH/POP,
            // или что они согласованы.
            SENKAID_ASSERT(false, "is_aligned: invalid alignment or null pointer");
            return false;
        }
        bool result = (reinterpret_cast<std::uintptr_t>(ptr) % alignment) == 0;
        return result;
    }

    // align_memory: Aligns a pointer upwards to the next boundary.
    // Note: This does not allocate new memory. The caller must ensure
    //       the underlying buffer is large enough.
    // Parameters:
    //   ptr       - The pointer to align.
    //   alignment - The required alignment (must be a power of two).
    //   size      - The size of the buffer (needed for std::align).
    // Returns:
    //   The aligned pointer, or the original pointer if alignment is invalid or fails.
    SENKAID_FORCE_INLINE void* align_memory(void* ptr, std::size_t alignment, std::size_t size) { // Убран SENKAID_DEBUG_ONLY

        if (SENKAID_UNLIKELY(ptr == nullptr || alignment == 0 || (alignment & (alignment - 1)) != 0)) {
            SENKAID_LOG_WARNING("align_memory: invalid input");
            SENKAID_ASSERT(false, "align_memory: invalid alignment or null pointer");
            return nullptr;
        }
        void* aligned_ptr = ptr;
        std::size_t space = size;
        if (std::align(alignment, sizeof(std::uint8_t), aligned_ptr, space) == nullptr) {
            SENKAID_LOG_WARNING("align_memory: alignment failed");
            SENKAID_ASSERT(false, "align_memory: insufficient space for alignment");
            return nullptr;
        }
        return aligned_ptr;
    }
#else

#endif

// is_valid_alignment: Compile-time check if a value is a power of two.
// Template parameter:
//   Alignment - The alignment value to check at compile time.
// Returns:
//   true if Alignment is a valid power of two, false otherwise.
#if defined(SENKAID_HAS_CONSTEVAL)
    template<std::size_t Alignment>
    consteval bool is_valid_alignment() {
        return Alignment > 0 && (Alignment & (Alignment - 1)) == 0;
    }
#else
    template<std::size_t Alignment>
    constexpr bool is_valid_alignment() {
        return Alignment > 0 && (Alignment & (Alignment - 1)) == 0;
    }
#endif

SENKAID_DIAGNOSTIC_POP

} // namespace senkaid::memory
