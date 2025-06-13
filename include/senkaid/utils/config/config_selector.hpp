#pragma once

/**
 * @file config_selector.hpp
 * @brief Configuration selection logic for the Senkaid library.
 *
 * Provides compile-time and runtime selection of backends, precision, and optimization
 * strategies. Integrates with compiler.hpp, platform.hpp, defines.hpp, and config_macros.hpp
 * to ensure consistent configuration across the library.
 */

#include "platform.hpp"
#include "compiler.hpp"
#include "config_macros.hpp"
#include <type_traits>

// ==========================================
// Backend Selection
// ==========================================

namespace senkaid::utils::config {
    // Backend enumeration for runtime dispatch
    enum class BackendType {
        CPU,
        CUDA,
        ROCM,
        SYCL
    };

    // Compile-time backend selection
    #if defined(SENKAID_DEFAULT_BACKEND_CUDA)
        inline constexpr BackendType default_backend = BackendType::CUDA;
    #elif defined(SENKAID_DEFAULT_BACKEND_ROCM)
        inline constexpr BackendType default_backend = BackendType::ROCM;
    #elif defined(SENKAID_DEFAULT_BACKEND_SYCL)
        inline constexpr BackendType default_backend = BackendType::SYCL;
    #else
        inline constexpr BackendType default_backend = BackendType::CPU;
    #endif

    // Runtime backend availability check
    template<BackendType backend>
    inline constexpr bool is_backend_available = false;

    template<> inline constexpr bool is_backend_available<BackendType::CPU> = true;
    template<> inline constexpr bool is_backend_available<BackendType::CUDA> = SENKAID_ENABLE_CUDA;
    template<> inline constexpr bool is_backend_available<BackendType::ROCM> = SENKAID_ENABLE_ROCM;
    template<> inline constexpr bool is_backend_available<BackendType::SYCL> = SENKAID_ENABLE_SYCL;

    // Select backend at compile-time if only one is available
    #if SENKAID_ENABLE_CUDA && !SENKAID_ENABLE_ROCM && !SENKAID_ENABLE_SYCL
        inline constexpr BackendType compile_time_backend = BackendType::CUDA;
    #elif SENKAID_ENABLE_ROCM && !SENKAID_ENABLE_CUDA && !SENKAID_ENABLE_SYCL
        inline constexpr BackendType compile_time_backend = BackendType::ROCM;
    #elif SENKAID_ENABLE_SYCL && !SENKAID_ENABLE_CUDA && !SENKAID_ENABLE_ROCM
        inline constexpr BackendType compile_time_backend = BackendType::SYCL;
    #else
        inline constexpr BackendType compile_time_backend = BackendType::CPU;
    #endif
} // namespace senkaid::utils::config

// ==========================================
// Precision Selection
// ==========================================

namespace senkaid::utils::config {
    // Default scalar type
    using default_scalar = SENKAID_DEFAULT_SCALAR;

    // Precision enumeration
    enum class PrecisionType {
        Half,
        Float,
        Double
    };

    // Compile-time precision selection
    #if defined(SENKAID_DEFAULT_PRECISION_HALF)
        #ifdef __CUDACC__
            #include <cuda_fp16.h>
            inline constexpr PrecisionType default_precision = PrecisionType::Half;
        #endif
    #elif defined(SENKAID_DEFAULT_PRECISION_FLOAT)
        inline constexpr PrecisionType default_precision = PrecisionType::Float;
    #else
        inline constexpr PrecisionType default_precision = PrecisionType::Double;
    #endif

    // Map precision to type
    template<PrecisionType precision>
    struct precision_to_type;
    #if defined(__CUDACC__)
        template<> struct precision_to_type<PrecisionType::Half> { using type = __half; };
    #endif
    template<> struct precision_to_type<PrecisionType::Float> { using type = float; };
    template<> struct precision_to_type<PrecisionType::Double> { using type = double; };

    // Type to precision
    template<typename T>
    inline constexpr PrecisionType type_to_precision = PrecisionType::Double;
    #if defined(__CUDACC__)
        template<> inline constexpr PrecisionType type_to_precision<__half> = PrecisionType::Half;
    #endif
    template<> inline constexpr PrecisionType type_to_precision<float> = PrecisionType::Float;
    template<> inline constexpr PrecisionType type_to_precision<double> = PrecisionType::Double;
} // namespace senkaid::utils::config

// ==========================================
// Optimization Strategy Selection
// ==========================================

namespace senkaid::utils::config {
    // Optimization level enumeration
    enum class OptimizationLevel {
        None,    // No optimizations
        Basic,   // Basic optimizations (e.g., inlining)
        Advanced // Advanced optimizations (e.g., SIMD, parallel)
    };

    // Select optimization level based on build configuration
    #if SENKAID_ENABLE_DEBUG
        inline constexpr OptimizationLevel optimization_level = OptimizationLevel::None;
    #elif SENKAID_ENABLE_PROFILING
        inline constexpr OptimizationLevel optimization_level = OptimizationLevel::Basic;
    #else
        inline constexpr OptimizationLevel optimization_level = OptimizationLevel::Advanced;
    #endif

    // SIMD usage
    inline constexpr bool use_simd =
        optimization_level == OptimizationLevel::Advanced &&
        (SENKAID_HAS_AVX || SENKAID_ARCH_AARCH64);

    // Parallelization usage
    inline constexpr bool use_parallel =
        optimization_level == OptimizationLevel::Advanced &&
        SENKAID_ENABLE_OPENMP &&
        (SENKAID_PARALLEL_MIN_SIZE > 0);

    // Memory alignment usage
    inline constexpr bool use_aligned_memory =
        SENKAID_USE_ALIGNED_ALLOC &&
        (optimization_level != OptimizationLevel::None);
} // namespace senkaid::utils::config

// ==========================================
// Feature Selection
// ==========================================

namespace senkaid::utils::config {
    // Feature availability
    inline constexpr bool use_sparse = SENKAID_ENABLE_SPARSE;
    inline constexpr bool use_autodiff = SENKAID_ENABLE_AUTODIFF;
    inline constexpr bool use_stats = SENKAID_ENABLE_STATS;
    inline constexpr bool use_fft = SENKAID_ENABLE_FFT;
    inline constexpr bool use_python = SENKAID_ENABLE_PYTHON;
} // namespace senkaid::utils::config

// ==========================================
// Fallback Configuration
// ==========================================

#if defined(SENKAID_COMPILER_UNKNOWN) || defined(SENKAID_PLATFORM_UNKNOWN) || defined(SENKAID_ARCH_UNKNOWN)
    #define SENKAID_USE_FALLBACK_CONFIG 1
#else
    #define SENKAID_USE_FALLBACK_CONFIG 0
#endif

#if SENKAID_USE_FALLBACK_CONFIG
    #undef SENKAID_ENABLE_OPENMP
    #define SENKAID_ENABLE_OPENMP 0
    #undef SENKAID_ENABLE_SPARSE
    #define SENKAID_ENABLE_SPARSE 0
    #undef SENKAID_ENABLE_AUTODIFF
    #define SENKAID_ENABLE_AUTODIFF 0
    #undef SENKAID_ENABLE_FFT
    #define SENKAID_ENABLE_FFT 0
    #undef SENKAID_USE_MEMORY_POOL
    #define SENKAID_USE_MEMORY_POOL 0
#endif

// ==========================================
// Compile-Time Validation
// ==========================================

#if SENKAID_USE_FALLBACK_CONFIG && !defined(SENKAID_DEFAULT_BACKEND_CPU)
    #error "Fallback configuration requires CPU backend"
#endif


