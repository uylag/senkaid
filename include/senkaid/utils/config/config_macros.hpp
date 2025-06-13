#pragma once

/**
 * @file config_macros.hpp
 * @brief Configuration and optimization macros for the Senkaid library.
 *
 * Defines macros for controlling build-time configurations, optimization hints,
 * and feature-specific settings. Complements compiler.hpp, platform.hpp, and
 * defines.hpp by providing fine-grained control over library behavior.
 */



// ==========================================
// Backend Configuration
// ==========================================

// Ensure only one GPU backend is enabled at a time
#if SENKAID_ENABLE_CUDA && SENKAID_ENABLE_ROCM
    #error "Cannot enable both CUDA and ROCm backends simultaneously"
#endif

#if SENKAID_ENABLE_CUDA && SENKAID_ENABLE_SYCL
    #error "Cannot enable both CUDA and SYCL backends simultaneously"
#endif

#if SENKAID_ENABLE_ROCM && SENKAID_ENABLE_SYCL
    #error "Cannot enable both ROCm and SYCL backends simultaneously"
#endif

// Default backend if none specified
#ifndef SENKAID_DEFAULT_BACKEND
    #if SENKAID_ENABLE_CUDA
        #define SENKAID_DEFAULT_BACKEND_CUDA 1
    #elif SENKAID_ENABLE_ROCM
        #define SENKAID_DEFAULT_BACKEND_ROCM 1
    #elif SENKAID_ENABLE_SYCL
        #define SENKAID_DEFAULT_BACKEND_SYCL 1
    #else
        #define SENKAID_DEFAULT_BACKEND_CPU 1
    #endif
#endif

// ==========================================
// Precision Configuration
// ==========================================

#ifndef SENKAID_DEFAULT_PRECISION
    #define SENKAID_DEFAULT_PRECISION_DOUBLE 1
#endif

#if defined(SENKAID_DEFAULT_PRECISION_DOUBLE)
    #define SENKAID_DEFAULT_SCALAR double
#elif defined(SENKAID_DEFAULT_PRECISION_FLOAT)
    #define SENKAID_DEFAULT_SCALAR float
#elif defined(SENKAID_DEFAULT_PRECISION_HALF)
    #define SENKAID_DEFAULT_SCALAR __half // CUDA/ROCm specific
#else
    #error "Invalid default precision specified"
#endif

// ==========================================
// Optimization Hints
// ==========================================

// Loop vectorization hint
#if defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_VECTORIZE SENKAID_PRAGMA(ivdep)
#else
    #define SENKAID_VECTORIZE
#endif

// Loop unrolling hint
#if defined(SENKAID_COMPILER_GCC)
    #define SENKAID_UNROLL(N) SENKAID_PRAGMA(unroll N)
#elif defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_UNROLL(N) SENKAID_PRAGMA(unroll N)
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_UNROLL(N) SENKAID_PRAGMA(loop(hint_parallel(N)))
#else
    #define SENKAID_UNROLL(N)
#endif

// Prefetch hint for cache optimization
#if defined(SENKAID_ARCH_X86_64) && (defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_CLANG))
    #define SENKAID_PREFETCH(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
#else
    #define SENKAID_PREFETCH(addr, rw, locality)
#endif

// ==========================================
// Memory Management Configuration
// ==========================================

#ifndef SENKAID_USE_MEMORY_POOL
    #define SENKAID_USE_MEMORY_POOL 1
#endif

#ifndef SENKAID_USE_ALIGNED_ALLOC
    #define SENKAID_USE_ALIGNED_ALLOC 1
#endif

// ==========================================
// Feature-Specific Toggles
// ==========================================

#ifndef SENKAID_ENABLE_SPARSE
    #define SENKAID_ENABLE_SPARSE 1
#endif

#ifndef SENKAID_ENABLE_AUTODIFF
    #define SENKAID_ENABLE_AUTODIFF 0
#endif

#ifndef SENKAID_ENABLE_STATS
    #define SENKAID_ENABLE_STATS 1
#endif

#ifndef SENKAID_ENABLE_FFT
    #define SENKAID_ENABLE_FFT 0
#endif

// ==========================================
// Parallelism Configuration
// ==========================================

#ifndef SENKAID_PARALLEL_MIN_SIZE
    #define SENKAID_PARALLEL_MIN_SIZE 1024
#endif

#if SENKAID_ENABLE_OPENMP
    #define SENKAID_PARALLEL_OPENMP SENKAID_PRAGMA(omp parallel for)
#else
    #define SENKAID_PARALLEL_OPENMP
#endif

// ==========================================
// Profiling and Debugging
// ==========================================

#ifndef SENKAID_ENABLE_PROFILING
    #define SENKAID_ENABLE_PROFILING 0
#endif

#if SENKAID_ENABLE_PROFILING
    #if defined(SENKAID_PLATFORM_LINUX)
        #define SENKAID_PROFILE_PERF 1
    #elif defined(SENKAID_PLATFORM_WINDOWS)
        #define SENKAID_PROFILE_VTUNE 1
    #else
        #define SENKAID_PROFILE_NONE 1
    #endif
#else
    #define SENKAID_PROFILE_NONE 1
#endif

#ifndef SENKAID_LOG_LEVEL
    #define SENKAID_LOG_LEVEL 0 // 0: None, 1: Error, 2: Warning, 3: Info
#endif

// ==========================================
// Testing Configuration
// ==========================================

#ifndef SENKAID_ENABLE_UNIT_TESTS
    #define SENKAID_ENABLE_UNIT_TESTS 1
#endif

#ifndef SENKAID_ENABLE_BENCHMARKS
    #define SENKAID_ENABLE_BENCHMARKS 1
#endif

// ==========================================
// Python Integration
// ==========================================

#ifndef SENKAID_ENABLE_PYTHON
    #define SENKAID_ENABLE_PYTHON 0
#endif

// ==========================================
// Numerical Stability
// ==========================================

#ifndef SENKAID_CHECK_NUMERICS
    #define SENKAID_CHECK_NUMERICS SENKAID_ENABLE_DEBUG
#endif

// ==========================================
// Matrix and Tensor Configuration
// ==========================================

#ifndef SENKAID_MAX_MATRIX_DIM
    #define SENKAID_MAX_MATRIX_DIM 1ULL << 50 // 2^30
#endif

#ifndef SENKAID_MAX_TENSOR_RANK
    #define SENKAID_MAX_TENSOR_RANK 20
#endif

// ==========================================
// Build-Time Validation
// ==========================================

#if defined(SENKAID_DEFAULT_BACKEND_CUDA) && !SENKAID_ENABLE_CUDA
    #error "CUDA backend selected but not enabled"
#endif

#if defined(SENKAID_DEFAULT_BACKEND_ROCM) && !SENKAID_ENABLE_ROCM
    #error "ROCm backend selected but not enabled"
#endif

#if defined(SENKAID_DEFAULT_BACKEND_SYCL) && !SENKAID_ENABLE_SYCL
    #error "SYCL backend selected but not enabled"
#endif

#if SENKAID_DEFAULT_PRECISION_HALF && !(SENKAID_ENABLE_CUDA || SENKAID_ENABLE_ROCM)
    #error "Half precision requires CUDA or ROCm backend"
#endif
