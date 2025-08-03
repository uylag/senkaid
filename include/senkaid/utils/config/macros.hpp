#pragma once

// macros.hpp: Defines project-wide configuration macros for the senkaid library.
// Controls feature toggles, precision, parallelism, memory management, and performance optimizations.
// All macros are prefixed with SENKAID_ to avoid conflicts and can be overridden via CMake.

// Feature toggles for backends
// SENKAID_ENABLE_CUDA: Enables CUDA backend for GPU acceleration.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_CUDA=ON).
#ifndef SENKAID_ENABLE_CUDA
    #define SENKAID_ENABLE_CUDA 0
#endif

// SENKAID_ENABLE_ROCM: Enables ROCm/HIP backend for AMD GPU acceleration.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_ROCM=ON).
#ifndef SENKAID_ENABLE_ROCM
    #define SENKAID_ENABLE_ROCM 0
#endif

// SENKAID_ENABLE_SYCL: Enables SYCL backend for cross-platform GPU/CPU acceleration.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_SYCL=ON).
#ifndef SENKAID_ENABLE_SYCL
    #define SENKAID_ENABLE_SYCL 0
#endif

// SENKAID_ENABLE_OPENMP: Enables OpenMP for CPU parallelism.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_OPENMP=ON).
#ifndef SENKAID_ENABLE_OPENMP
    #define SENKAID_ENABLE_OPENMP 0
#endif

// SENKAID_ENABLE_TBB: Enables Intel TBB for CPU parallelism.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_TBB=ON).
#ifndef SENKAID_ENABLE_TBB
    #define SENKAID_ENABLE_TBB 0
#endif

// Precision control
// SENKAID_DEFAULT_FLOAT: Default floating-point type for matrix/tensor operations.
// Default: double for high precision, can be overridden to float for performance.
#ifndef SENKAID_DEFAULT_FLOAT
    #define SENKAID_DEFAULT_FLOAT double
#endif

// SENKAID_DEFAULT_INT: Default integer type for indexing and integer operations.
// Default: int32_t for compatibility with most platforms.
#ifndef SENKAID_DEFAULT_INT
    #define SENKAID_DEFAULT_INT int32_t
#endif

// SENKAID_ENABLE_MIXED_PRECISION: Enables mixed-precision arithmetic (FP16, BF16, TF32).
// Default: Disabled to ensure numerical stability unless explicitly enabled.
#ifndef SENKAID_ENABLE_MIXED_PRECISION
    #define SENKAID_ENABLE_MIXED_PRECISION 0
#endif

// SENKAID_STRICT_IEEE754: Enforces strict IEEE 754 compliance for floating-point operations.
// Default: Enabled to ensure precision, can be disabled for performance with SENKAID_FAST_MATH.
#ifndef SENKAID_STRICT_IEEE754
    #define SENKAID_STRICT_IEEE754 1
#endif

// SENKAID_USE_COMPENSATED_SUMMATION: Enables Kahan/Babushka-Neumaier summation for improved precision.
// Default: Disabled unless SENKAID_STRICT_IEEE754 is enabled.
#if SENKAID_STRICT_IEEE754
    #define SENKAID_USE_COMPENSATED_SUMMATION 1
#else
    #define SENKAID_USE_COMPENSATED_SUMMATION 0
#endif

// Debug and verification
// SENKAID_DEBUG: Enables debug checks (assertions, logging) for development.
// Default: Enabled if NDEBUG is not defined, disabled in release builds.
#ifdef NDEBUG
    #define SENKAID_DEBUG 0
#else
    #define SENKAID_DEBUG 1
#endif

// SENKAID_VERIFY_NUMERICS: Enables runtime checks for NaNs/INFs in numerical operations.
// Default: Enabled in debug builds, disabled in release builds for performance.
#if SENKAID_DEBUG
    #define SENKAID_VERIFY_NUMERICS 1
#else
    #define SENKAID_VERIFY_NUMERICS 0
#endif

// SENKAID_USE_INTERVAL: Enables interval arithmetic for numerical validation.
// Default: Disabled unless explicitly enabled via CMake (-DSENKAID_USE_INTERVAL=ON).
#ifndef SENKAID_USE_INTERVAL
    #define SENKAID_USE_INTERVAL 0
#endif

// Memory management
// SENKAID_USE_MEMORY_POOL: Enables memory pools for temporary buffers to reduce allocation overhead.
// Default: Enabled for performance, can be disabled for simpler memory management.
#ifndef SENKAID_USE_MEMORY_POOL
    #define SENKAID_USE_MEMORY_POOL 1
#endif

// SENKAID_USE_COW: Enables copy-on-write semantics for large matrices/tensors.
// Default: Enabled to minimize memory copying, can be disabled for simplicity.
#ifndef SENKAID_USE_COW
    #define SENKAID_USE_COW 1
#endif

// SENKAID_DEFAULT_MEMORY_LAYOUT: Specifies default memory layout (row-major or column-major).
// Default: Column-major for BLAS compatibility, can be overridden to row-major.
#define SENKAID_MEMORY_LAYOUT_ROW_MAJOR 0
#define SENKAID_MEMORY_LAYOUT_COLUMN_MAJOR 1

#ifndef SENKAID_DEFAULT_MEMORY_LAYOUT
    #define SENKAID_DEFAULT_MEMORY_LAYOUT SENKAID_MEMORY_LAYOUT_COLUMN_MAJOR
#endif

// SENKAID_DEFAULT_ALIGNMENT: Default memory alignment for SIMD and GPU compatibility.
// Default: 64 bytes for cache-line alignment (AVX-512).
#ifndef SENKAID_DEFAULT_ALIGNMENT
    #if defined(SENKAID_ARCH_X86_64)
        #define SENKAID_DEFAULT_ALIGNMENT 64
    #elif defined(SENKAID_ARCH_AARCH64) || defined(SENKAID_ARCH_X86)
        #define SENKAID_DEFAULT_ALIGNMENT 32
    #else
        #define SENKAID_DEFAULT_ALIGNMENT 16
    #endif
#endif

// Performance optimizations
// SENKAID_ENABLE_PGO: Enables profile-guided optimization for hot paths.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_PGO=ON).
#ifndef SENKAID_ENABLE_PGO
    #define SENKAID_ENABLE_PGO 0
#endif

// SENKAID_ENABLE_JIT_KERNEL: Enables JIT kernel compilation for specialized matrix operations.
// Default: Disabled unless integrated with MLIR/TVM via CMake.
#ifndef SENKAID_ENABLE_JIT_KERNEL
    #define SENKAID_ENABLE_JIT_KERNEL 0
#endif

// SENKAID_ENABLE_LAZY_EXECUTION: Enables lazy execution with graph-based scheduling.
// Default: Disabled unless explicitly enabled via CMake (-DSENKAID_ENABLE_LAZY_EXECUTION=ON).
#ifndef SENKAID_ENABLE_LAZY_EXECUTION
    #define SENKAID_ENABLE_LAZY_EXECUTION 0
#endif

// SENKAID_ENABLE_KERNEL_FUSION: Enables kernel fusion for chained operations (e.g., A @ B + C).
// Default: Enabled when lazy execution is enabled, otherwise disabled.
#if SENKAID_ENABLE_LAZY_EXECUTION
    #define SENKAID_ENABLE_KERNEL_FUSION 1
#else
    #define SENKAID_ENABLE_KERNEL_FUSION 0
#endif

// SENKAID_DEFAULT_UNROLL_FACTOR: Default loop unroll factor for performance-critical loops.
// Default: 4 to balance code size and performance.
#ifndef SENKAID_DEFAULT_UNROLL_FACTOR
    #define SENKAID_DEFAULT_UNROLL_FACTOR 4
#endif

// SENKAID_DEFAULT_TILE_SIZE: Default tile size for cache blocking in matrix operations.
// Default: 64 to match cache line size, can be tuned via CMake.
#ifndef SENKAID_DEFAULT_TILE_SIZE
    #define SENKAID_DEFAULT_TILE_SIZE 64
#endif

// Type safety and constraints
// SENKAID_ENFORCE_TYPE_SAFETY: Enables strict type checks using C++20 concepts.
// Default: Enabled if concepts are supported (SENKAID_HAS_CONCEPTS), otherwise disabled.
#ifdef SENKAID_HAS_CONCEPTS
    #define SENKAID_ENFORCE_TYPE_SAFETY 1
#else
    #define SENKAID_ENFORCE_TYPE_SAFETY 0
#endif

// API and interoperability
// SENKAID_ENABLE_PYTHON_BINDINGS: Enables pybind11 bindings for Python integration.
// Default: Disabled unless set via CMake (-DSENKAID_ENABLE_PYTHON_BINDINGS=ON).
#ifndef SENKAID_ENABLE_PYTHON_BINDINGS
    #define SENKAID_ENABLE_PYTHON_BINDINGS 0
#endif

// SENKAID_ENABLE_INTEROP: Enables interoperability with cuBLAS, Eigen, and NumPy.
// Default: Enabled to support data exchange, can be disabled for minimal builds.
#ifndef SENKAID_ENABLE_INTEROP
    #define SENKAID_ENABLE_INTEROP 1
#endif

// Numerical algorithms
// SENKAID_ENABLE_BLOCKED_SOLVERS: Enables blocked solvers (QR, LU, Cholesky) for large tensors.
// Default: Enabled for performance, can be disabled for smaller binary size.
#ifndef SENKAID_ENABLE_BLOCKED_SOLVERS
    #define SENKAID_ENABLE_BLOCKED_SOLVERS 1
#endif

// SENKAID_ENABLE_ADAPTIVE_PRECISION: Enables adaptive precision switching during computation.
// Default: Disabled unless mixed precision is enabled.
#if SENKAID_ENABLE_MIXED_PRECISION
    #define SENKAID_ENABLE_ADAPTIVE_PRECISION 1
#else
    #define SENKAID_ENABLE_ADAPTIVE_PRECISION 0
#endif

// Profiling and debugging
// SENKAID_PROFILE: Enables profiling hooks for performance analysis (e.g., Linux perf, Tracy).
// Default: Disabled unless set via CMake (-DSENKAID_PROFILE=ON).
#ifndef SENKAID_PROFILE
    #define SENKAID_PROFILE 0
#endif

// SENKAID_PROFILE_BEGIN/END: Macros for profiling specific code sections.
// Disabled in non-profiling builds to avoid overhead.
// Implementation depends on selected profiling tool (e.g., Tracy, perf, Intel Vtune, etc.)
#if SENKAID_PROFILE
    #define SENKAID_PROFILE_BEGIN(name) // Hook into perf/Tracy (implementation depends on tool)
    #define SENKAID_PROFILE_END() // End profiling scope
#else
    #define SENKAID_PROFILE_BEGIN(name)
    #define SENKAID_PROFILE_END()
#endif

// Ownership semantics
// SENKAID_DISABLE_COPY: Disables copy constructors for matrices/tensors to enforce move-only semantics.
// Default: Disabled to allow flexibility, can be enabled for strict ownership.
#ifndef SENKAID_DISABLE_COPY
    #define SENKAID_DISABLE_COPY 0
#endif

// SENKAID_ENABLE_MOVE_ONLY: Forces move-only semantics for large data structures.
// Default: Enabled when copy-on-write is disabled to reduce overhead.
#if !SENKAID_USE_COW
    #define SENKAID_ENABLE_MOVE_ONLY 1
#else
    #define SENKAID_ENABLE_MOVE_ONLY 0
#endif
