#pragma once

// tools.hpp: Debugging and diagnostic utilities for the senkaid library.
// Provides tools for memory validation, performance timing, and data inspection, surpassing Eigen’s limited debugging facilities.
// Uses macros from compiler.hpp for compiler detection and debugging support.

#include "utils/debug/dump.hpp"
#include <cmath>

// Conditional includes for portability
#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint> // For std::uint64_t
    #endif
    #if SENKAID_HAS_INCLUDE(<chrono>)
        #include <chrono> // For performance timing
    #endif
    #if SENKAID_HAS_INCLUDE(<cstring>)
        #include <cstring> // For std::memcmp
    #endif
#else
    #include <cstdint>
    #include <chrono>
    #include <cstring>
#endif

namespace senkaid::debug {

SENKAID_DIAGNOSTIC_PUSH
SENKAID_IGNORE_WARNING_UNUSED_FUNCTION

// Memory validation
// Validates that a memory block contains expected values (e.g., for zero-initialization checks).
SENKAID_DEBUG_ONLY(
    template<typename T>
    SENKAID_FORCE_INLINE bool validate_memory(const T* ptr, std::size_t size, const T& expected) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            return false;
        }
        for (std::size_t i = 0; i < size; ++i) {
            if (SENKAID_UNLIKELY(ptr[i] != expected)) {
                return false;
            }
        }
        return true;
    }
)

// Memory comparison
// Compares two memory blocks for equality (e.g., for matrix/tensor validation).
SENKAID_DEBUG_ONLY(
    template<typename T>
    SENKAID_FORCE_INLINE bool compare_memory(const T* a, const T* b, std::size_t size) {
        if (SENKAID_UNLIKELY(a == nullptr || b == nullptr)) {
            return false;
        }
        return std::memcmp(a, b, size * sizeof(T)) == 0;
    }
)

// Performance timer
// Measures elapsed time for a code block in nanoseconds, useful for profiling matrix/tensor operations.
class Timer {
public:
    SENKAID_FORCE_INLINE Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    SENKAID_FORCE_INLINE std::uint64_t elapsed_ns() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
    }

    SENKAID_FORCE_INLINE double elapsed_ms() const {
        return static_cast<double>(elapsed_ns()) / 1'000'000.0;
    }

    SENKAID_FORCE_INLINE void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Bounds validation
// Validates matrix/tensor indices against dimensions.
SENKAID_DEBUG_ONLY(
    SENKAID_FORCE_INLINE bool validate_bounds(std::size_t index, std::size_t size) {
        return SENKAID_LIKELY(index < size);
    }
)

// CUDA memory validation (for backend/cuda)
#if defined(SENKAID_COMPILER_NVCC)
    SENKAID_CUDA_HOST_DEVICE SENKAID_DEBUG_ONLY(
        inline bool validate_cuda_memory(const float* ptr, std::size_t size, float expected) {
            if (!ptr) {
                return false;
            }
            for (std::size_t i = 0; i < size; ++i) {
                if (ptr[i] != expected) {
                    return false;
                }
            }
            return true;
        }
    )
#else
    SENKAID_DEBUG_ONLY(
        inline bool validate_cuda_memory(const float*, std::size_t, float) {
            return true; // No-op on non-CUDA builds
        }
    )
#endif

// NaN/Inf detection
// Checks a memory block for NaN or Inf values, critical for numerical stability.
SENKAID_DEBUG_ONLY(
    template<typename T>
    SENKAID_FORCE_INLINE bool has_nan_inf(const T* ptr, std::size_t size) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            return true;
        }
        for (std::size_t i = 0; i < size; ++i) {
            if (SENKAID_UNLIKELY(std::isnan(ptr[i]) || std::isinf(ptr[i]))) {
                return true;
            }
        }
        return false;
    }
)

// Compile-time dimension check
// Validates matrix/tensor dimensions at compile time using C++20 consteval.
#if defined(SENKAID_HAS_CONSTEVAL)
    template<std::size_t Dim, std::size_t MaxDim>
    consteval bool check_dimension() {
        if (Dim > MaxDim) {
            static_assert(false, "Dimension exceeds maximum allowed value");
        }
        return true;
    }
#else
    template<std::size_t Dim, std::size_t MaxDim>
    constexpr bool check_dimension() {
        return Dim <= MaxDim;
    }
#endif

SENKAID_DIAGNOSTIC_POP

} // namespace senkaid::debug
