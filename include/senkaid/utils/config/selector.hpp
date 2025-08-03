#pragma once

// selector.hpp: Defines macros and types for selecting configurations and implementations in the senkaid library.
// Handles backend selection (CPU, CUDA, ROCm, SYCL), precision, layout, SIMD, and kernel dispatch.
// Self-contained with no dependencies on other utils/config/*.hpp files; all macros have defaults.

// --- Configuration Macros ---

#ifndef SENKAID_ENABLE_CUDA
    #define SENKAID_ENABLE_CUDA 0
#endif
#ifndef SENKAID_ENABLE_ROCM
    #define SENKAID_ENABLE_ROCM 0
#endif
#ifndef SENKAID_ENABLE_SYCL
    #define SENKAID_ENABLE_SYCL 0
#endif
#ifndef SENKAID_ENABLE_MIXED_PRECISION
    #define SENKAID_ENABLE_MIXED_PRECISION 0
#endif
#ifndef SENKAID_DEFAULT_FLOAT
    #define SENKAID_DEFAULT_FLOAT double
#endif
#ifndef SENKAID_ENABLE_KERNEL_FUSION
    #define SENKAID_ENABLE_KERNEL_FUSION 0
#endif

#ifndef SENKAID_HAS_CUDA
    #define SENKAID_HAS_CUDA 0
#endif
#ifndef SENKAID_HAS_ROCM
    #define SENKAID_HAS_ROCM 0
#endif
#ifndef SENKAID_HAS_SYCL
    #define SENKAID_HAS_SYCL 0
#endif

#ifndef SENKAID_MEMORY_LAYOUT_ROW_MAJOR
    #define SENKAID_MEMORY_LAYOUT_ROW_MAJOR 0
#endif
#ifndef SENKAID_MEMORY_LAYOUT_COLUMN_MAJOR
    #define SENKAID_MEMORY_LAYOUT_COLUMN_MAJOR 1
#endif
#ifndef SENKAID_DEFAULT_MEMORY_LAYOUT
    #define SENKAID_DEFAULT_MEMORY_LAYOUT SENKAID_MEMORY_LAYOUT_COLUMN_MAJOR
#endif

// --- Core Selector Logic ---

enum class SenkaidBackend {
    CPU,
    CUDA,
    ROCM,
    SYCL
};

#if SENKAID_ENABLE_CUDA && SENKAID_HAS_CUDA
    #define SENKAID_DEFAULT_BACKEND SenkaidBackend::CUDA
#elif SENKAID_ENABLE_ROCM && SENKAID_HAS_ROCM
    #define SENKAID_DEFAULT_BACKEND SenkaidBackend::ROCM
#elif SENKAID_ENABLE_SYCL && SENKAID_HAS_SYCL
    #define SENKAID_DEFAULT_BACKEND SenkaidBackend::SYCL
#else
    #define SENKAID_DEFAULT_BACKEND SenkaidBackend::CPU
#endif

#define SENKAID_BACKEND_AVAILABLE(backend) \
    ((backend) == SenkaidBackend::CPU || \
     ((backend) == SenkaidBackend::CUDA && SENKAID_ENABLE_CUDA && SENKAID_HAS_CUDA) || \
     ((backend) == SenkaidBackend::ROCM && SENKAID_ENABLE_ROCM && SENKAID_HAS_ROCM) || \
     ((backend) == SenkaidBackend::SYCL && SENKAID_ENABLE_SYCL && SENKAID_HAS_SYCL))

#define SENKAID_SELECT_BACKEND(backend, cpu_func, cuda_func, rocm_func, sycl_func) \
    do { \
        switch (backend) { \
            case SenkaidBackend::CPU: \
                cpu_func; \
                break; \
            case SenkaidBackend::CUDA: \
                if (SENKAID_BACKEND_AVAILABLE(SenkaidBackend::CUDA)) { \
                    cuda_func; \
                } else { \
                    cpu_func; \
                } \
                break; \
            case SenkaidBackend::ROCM: \
                if (SENKAID_BACKEND_AVAILABLE(SenkaidBackend::ROCM)) { \
                    rocm_func; \
                } else { \
                    cpu_func; \
                } \
                break; \
            case SenkaidBackend::SYCL: \
                if (SENKAID_BACKEND_AVAILABLE(SenkaidBackend::SYCL)) { \
                    sycl_func; \
                } else { \
                    cpu_func; \
                } \
                break; \
        } \
    } while (0)

#if defined(__cpp_if_constexpr)
    #define SENKAID_IF_CONSTEXPR_BACKEND(backend, cpu_code, cuda_code, rocm_code, sycl_code) \
        if constexpr ((backend) == SenkaidBackend::CPU) { \
            cpu_code; \
        } else if constexpr ((backend) == SenkaidBackend::CUDA && SENKAID_ENABLE_CUDA && SENKAID_HAS_CUDA) { \
            cuda_code; \
        } else if constexpr ((backend) == SenkaidBackend::ROCM && SENKAID_ENABLE_ROCM && SENKAID_HAS_ROCM) { \
            rocm_code; \
        } else if constexpr ((backend) == SenkaidBackend::SYCL && SENKAID_ENABLE_SYCL && SENKAID_HAS_SYCL) { \
            sycl_code; \
        } else { \
            cpu_code; \
        }
#else
    #define SENKAID_IF_CONSTEXPR_BACKEND(backend, cpu_code, cuda_code, rocm_code, sycl_code) \
        SENKAID_SELECT_BACKEND(backend, cpu_code, cuda_code, rocm_code, sycl_code)
#endif

enum class Precision {
    FP16,
    BF16,
    FP32,
    FP64
};

#if SENKAID_ENABLE_MIXED_PRECISION
    #define SENKAID_DEFAULT_PRECISION float
#else
    #define SENKAID_DEFAULT_PRECISION SENKAID_DEFAULT_FLOAT
#endif

#define SENKAID_SELECT_PRECISION(precision, fp16_code, bf16_code, fp32_code, fp64_code) \
    do { \
        switch (precision) { \
            case Precision::FP16: \
                if (SENKAID_ENABLE_MIXED_PRECISION) { \
                    fp16_code; \
                } else { \
                    fp32_code; \
                } \
                break; \
            case Precision::BF16: \
                if (SENKAID_ENABLE_MIXED_PRECISION) { \
                    bf16_code; \
                } else { \
                    fp32_code; \
                } \
                break; \
            case Precision::FP32: \
                fp32_code; \
                break; \
            case Precision::FP64: \
                fp64_code; \
                break; \
        } \
    } while (0)

#define SENKAID_MATRIX_LAYOUT SENKAID_DEFAULT_MEMORY_LAYOUT

#if defined(__cpp_if_constexpr)
    #define SENKAID_IF_CONSTEXPR_LAYOUT(layout, row_major_code, column_major_code) \
        if constexpr ((layout) == SENKAID_MEMORY_LAYOUT_ROW_MAJOR) { \
            row_major_code; \
        } else { \
            column_major_code; \
        }
#else
    #define SENKAID_IF_CONSTEXPR_LAYOUT(layout, row_major_code, column_major_code) \
        do { \
            if ((layout) == SENKAID_MEMORY_LAYOUT_ROW_MAJOR) { \
                row_major_code; \
            } else { \
                column_major_code; \
            } \
        } while (0)
#endif

#define SENKAID_SELECT_SIMD(sse_code, avx_code, avx512_code, neon_code, scalar_code) \
    do { \
        if (SENKAID_HAS_AVX512) { \
            avx512_code; \
        } else if (SENKAID_HAS_AVX2) { \
            avx_code; \
        } else if (SENKAID_HAS_SSE2) { \
            sse_code; \
        } else if (SENKAID_HAS_NEON) { \
            neon_code; \
        } else { \
            scalar_code; \
        } \
    } while (0)

#if defined(__cpp_if_constexpr)
    #define SENKAID_IF_CONSTEXPR_SIMD(sse_code, avx_code, avx512_code, neon_code, scalar_code) \
        if constexpr (SENKAID_HAS_AVX512) { \
            avx512_code; \
        } else if constexpr (SENKAID_HAS_AVX2) { \
            avx_code; \
        } else if constexpr (SENKAID_HAS_SSE2) { \
            sse_code; \
        } else if constexpr (SENKAID_HAS_NEON) { \
            neon_code; \
        } else { \
            scalar_code; \
        }
#else
    #define SENKAID_IF_CONSTEXPR_SIMD(sse_code, avx_code, avx512_code, neon_code, scalar_code) \
        SENKAID_SELECT_SIMD(sse_code, avx_code, avx512_code, neon_code, scalar_code)
#endif

#define SENKAID_FUSE_KERNEL(fused_code, unfused_code) \
    do { \
        if (SENKAID_ENABLE_KERNEL_FUSION) { \
            fused_code; \
        } else { \
            unfused_code; \
        } \
    } while (0)

#define SENKAID_DISPATCH_KERNEL(backend, sse_func, avx_func, avx512_func, neon_func, cuda_func, rocm_func, sycl_func, scalar_func) \
    do { \
        if ((backend) == SenkaidBackend::CPU) { \
            SENKAID_SELECT_SIMD(sse_func, avx_func, avx512_func, neon_func, scalar_func); \
        } else if ((backend) == SenkaidBackend::CUDA && SENKAID_BACKEND_AVAILABLE(SenkaidBackend::CUDA)) { \
            cuda_func; \
        } else if ((backend) == SenkaidBackend::ROCM && SENKAID_BACKEND_AVAILABLE(SenkaidBackend::ROCM)) { \
            rocm_func; \
        } else if ((backend) == SenkaidBackend::SYCL && SENKAID_BACKEND_AVAILABLE(SenkaidBackend::SYCL)) { \
            sycl_func; \
        } else { \
            SENKAID_SELECT_SIMD(sse_func, avx_func, avx512_func, neon_func, scalar_func); \
        } \
    } while (0)
