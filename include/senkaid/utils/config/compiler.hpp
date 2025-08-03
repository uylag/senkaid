#pragma once

// compiler.hpp: Compiler detection and performance-critical macros for the senkaid library.
// Defines macros for compiler identification, attributes, SIMD support, and optimization hints.

#define SENKAID_STRINGIFY(x) #x

// Compiler detection
// Identifies the compiler and its version for tailored optimizations and workarounds.
#if defined(__clang__)
    // Clang compiler (Apple Clang or LLVM Clang).
    #define SENKAID_COMPILER_CLANG
    #define SENKAID_COMPILER_NAME "Clang"
    #define SENKAID_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(__GNUC__) && !defined(__clang__)
    // GNU Compiler Collection (GCC).
    #define SENKAID_COMPILER_GCC
    #define SENKAID_COMPILER_NAME "GCC"
    #define SENKAID_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
    // Microsoft Visual C++ (MSVC).
    #define SENKAID_COMPILER_MSVC
    #define SENKAID_COMPILER_NAME "MSVC"
    #define SENKAID_COMPILER_VERSION _MSC_VER
#elif defined(__INTEL_COMPILER)
    // Intel C++ Compiler (ICC).
    #define SENKAID_COMPILER_INTEL
    #define SENKAID_COMPILER_NAME "Intel C++"
    #define SENKAID_COMPILER_VERSION __INTEL_COMPILER
#elif defined(__NVCC__)
    // NVIDIA CUDA Compiler (NVCC).
    #define SENKAID_COMPILER_NVCC
    #define SENKAID_COMPILER_NAME "NVCC"
    #define SENKAID_COMPILER_VERSION (__CUDACC_VER_MAJOR__ * 10000 + __CUDACC_VER_MINOR__ * 100 + __CUDACC_VER_BUILD__)
#else
    // Unknown compiler (fallback).
    #define SENKAID_COMPILER_UNKNOWN
    #define SENKAID_COMPILER_NAME "Unknown"
    #define SENKAID_COMPILER_VERSION 0
#endif

// Minimum C++ standard check
// senkaid requires C++20 for modern features like concepts and std::bit_cast.
#if __cplusplus < 202002L
    #error "senkaid requires at least C++20"
#endif

#if defined(SENKAID_COMPILER_CLANG) && (SENKAID_COMPILER_VERSION < 100000) // Clang 10.0.0
    #error "Clang version 10.0 or higher is required for C++20 support"
#elif defined(SENKAID_COMPILER_GCC) && (SENKAID_COMPILER_VERSION < 100000) // GCC 10.0.0
    #error "GCC version 10.0 or higher is required for C++20 support"
#elif defined(SENKAID_COMPILER_MSVC) && (SENKAID_COMPILER_VERSION < 1920) // VS 2019
    #error "MSVC version 2019 (16.0) or higher is required for C++20 support"
#endif

// Compiler attributes for performance and correctness
// SENKAID_NODISCARD: Marks functions whose return value should not be ignored.
// Supported: Clang/GCC (C++17 [[nodiscard]]), MSVC (_Check_return_).
#if __has_cpp_attribute(nodiscard)
    #define SENKAID_NODISCARD [[nodiscard]]
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_NODISCARD _Check_return_
#else
    #define SENKAID_NODISCARD
#endif

// SENKAID_NORETURN: Marks functions that never return.
// Supported: Clang/GCC ([[noreturn]]), MSVC (__declspec(noreturn)).
#if __has_cpp_attribute(noreturn)
    #define SENKAID_NORETURN [[noreturn]]
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_NORETURN __declspec(noreturn)
#else
    #define SENKAID_NORETURN
#endif

// SENKAID_DEPRECATED: Marks deprecated functions or types.
// Supported: Clang/GCC ([[deprecated]]), MSVC (__declspec(deprecated)).
#if __has_cpp_attribute(deprecated)
    #define SENKAID_DEPRECATED [[deprecated]]
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_DEPRECATED __declspec(deprecated)
#else
    #define SENKAID_DEPRECATED
#endif

// Branch prediction optimization
// SENKAID_LIKELY(x): Hints that a condition is likely true for branch prediction.
// SENKAID_UNLIKELY(x): Hints that a condition is likely false.
// Supported: Clang/GCC (__builtin_expect), no-op for others.
#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_LIKELY(x) __builtin_expect(!!(x), 1)
    #define SENKAID_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define SENKAID_LIKELY(x) (x)
    #define SENKAID_UNLIKELY(x) (x)
#endif

// Inlining control
// SENKAID_FORCE_INLINE: Forces inlining for performance-critical functions.
// SENKAID_NO_INLINE: Prevents inlining for debugging or large functions.
// Supported: Clang/GCC (__attribute__((always_inline/noinline))), MSVC (__forceinline/__declspec(noinline)).
#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_FORCE_INLINE inline __attribute__((always_inline))
    #define SENKAID_NO_INLINE __attribute__((noinline))
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_FORCE_INLINE __forceinline
    #define SENKAID_NO_INLINE __declspec(noinline)
#else
    #define SENKAID_FORCE_INLINE inline
    #define SENKAID_NO_INLINE
#endif

// Code path optimization
// SENKAID_HOT: Marks frequently executed code paths.
// SENKAID_COLD: Marks rarely executed code paths.
// Supported: Clang/GCC (__attribute__((hot/cold))), no-op for others.
#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_HOT __attribute__((hot))
    #define SENKAID_COLD __attribute__((cold))
#else
    #define SENKAID_HOT
    #define SENKAID_COLD
#endif

// Data alignment
// SENKAID_ALIGNED(N): Specifies alignment for data structures (e.g., cache-line alignment for SIMD).
// Supported: Clang/GCC (__attribute__((aligned(N)))), MSVC (__declspec(align(N))).
#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_ALIGNED(N) __attribute__((aligned(N)))
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_ALIGNED(N) __declspec(align(N))
#else
    #define SENKAID_ALIGNED(N)
#endif

// SIMD instruction set detection
// Detects support for x86/x86_64 (SSE, AVX) and ARM (NEON) instruction sets.
#if defined(__SSE__)
    #define SENKAID_HAS_SSE
#endif
#if defined(__SSE2__)
    #define SENKAID_HAS_SSE2
#endif
#if defined(__SSE4_2__)
    #define SENKAID_HAS_SSE4_2
#endif
#if defined(__AVX__)
    #define SENKAID_HAS_AVX
#endif
#if defined(__AVX2__)
    #define SENKAID_HAS_AVX2
#endif
#if defined(__AVX512F__)
    #define SENKAID_HAS_AVX512
#endif
#if defined(__ARM_NEON)
    #define SENKAID_HAS_NEON
#endif

// Header availability
// SENKAID_HAS_INCLUDE(header): Checks if a header is available using __has_include.
#if defined(__has_include)
    #define SENKAID_HAS_INCLUDE(header) __has_include(header)
#else
    #define SENKAID_HAS_INCLUDE(header) 0
#endif

// Standard C++ headers
#if SENKAID_HAS_INCLUDE(<version>)
    #define SENKAID_HAS_INCLUDE_VERSION
    #include <version>
#endif

#if SENKAID_HAS_INCLUDE(<concepts>) && (!defined(SENKAID_COMPILER_NVCC) || (defined(SENKAID_COMPILER_NVCC) && SENKAID_COMPILER_VERSION >= 120000)) // Assuming CUDA 11.2+ supports concepts
    #define SENKAID_HAS_CONCEPTS
#else
    #define SENKAID_HAS_CONCEPTS 0
#endif

#if SENKAID_HAS_INCLUDE(<bit>)
    #define SENKAID_HAS_BIT
#endif

#if SENKAID_HAS_INCLUDE(<thread>)
    #define SENKAID_HAS_CXX_THREAD
#endif

#if SENKAID_HAS_INCLUDE(<atomic>)
    #define SENKAID_HAS_CXX_ATOMIC
#endif

// SIMD headers
#if SENKAID_HAS_INCLUDE(<immintrin.h>)
    #define SENKAID_HAS_IMMINTRIN_H
#endif

#if SENKAID_HAS_INCLUDE(<x86intrin.h>)
    #define SENKAID_HAS_X86INTRIN_H
#endif

#if SENKAID_HAS_INCLUDE(<arm_neon.h>)
    #define SENKAID_HAS_NEON_H
#endif

// Platform-specific headers
#if SENKAID_HAS_INCLUDE(<pthread.h>)
    #define SENKAID_HAS_PTHREAD
#endif

#if SENKAID_HAS_INCLUDE(<windows.h>)
    #define SENKAID_HAS_WINDOWS
#endif

#if SENKAID_HAS_INCLUDE(<cuda_runtime.h>)
    #define SENKAID_HAS_CUDA_RUNTIME
#endif

#ifndef SENKAID_ALLOW_BROKEN_PLATFORM
    // Required: <version>
    #ifndef SENKAID_HAS_INCLUDE_VERSION
        #error "[senkaid] <version> header not found. Update compiler/libc++ or define SENKAID_ALLOW_BROKEN_PLATFORM"
    #endif
    // Required: <concepts>
    #ifndef SENKAID_HAS_CONCEPTS
        #error "[senkaid] Concepts not supported. Requires C++20 concepts. Update compiler (or CUDA 11.2+ for NVCC)"
    #endif
    // Required: <bit>
    #ifndef SENKAID_HAS_BIT
        #error "[senkaid] <bit> header not found. Required for bitmask operations and SIMD. Use C++20 or define SENKAID_ALLOW_BROKEN_PLATFORM"
    #endif
    // Required: <thread>
    #ifndef SENKAID_HAS_CXX_THREAD
        #error "[senkaid] <thread> header missing. Multithreading is required. Use modern libc++ or define SENKAID_ALLOW_BROKEN_PLATFORM"
    #endif
    // Required: <atomic>
    #ifndef SENKAID_HAS_CXX_ATOMIC
        #error "[senkaid] <atomic> header missing. Needed for backend safety. Update your toolchain"
    #endif
    // Required: <immintrin.h>
    #ifndef SENKAID_HAS_IMMINTRIN_H
        #error "[senkaid] <immintrin.h> (SIMD intrinsics) not available. AVX/SSE code paths will fail"
    #endif
#endif // SENKAID_ALLOW_BROKEN_PLATFORM


// C++20 feature detection
// Uses <version> macros for precise feature detection.
#ifdef SENKAID_HAS_INCLUDE_VERSION
    #ifdef __cpp_lib_bit_cast
        #define SENKAID_HAS_BIT_CAST
    #endif
    #ifdef __cpp_consteval
        #define SENKAID_HAS_CONSTEVAL
    #endif
    #ifdef __cpp_constinit
        #define SENKAID_HAS_CONSTINIT
    #endif
#endif

// Compiler-specific optimizations
// SENKAID_FAST_MATH: Enables fast floating-point optimizations (e.g., -ffast-math for Clang/GCC, /fp:fast for MSVC via CMake).
// Warning: May affect numerical precision.
#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_FAST_MATH __attribute__((optimize("fast-math")))
#else
    #define SENKAID_FAST_MATH
#endif

// SENKAID_UNROLL_LOOPS: Forces loop unrolling for small, performance-critical loops.
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_UNROLL_LOOPS _Pragma("clang loop unroll(full)")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_UNROLL_LOOPS _Pragma("GCC unroll 4")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_UNROLL_LOOPS _Pragma("loop(unroll)")
#else
    #define SENKAID_UNROLL_LOOPS
#endif

// SENKAID_VECTORIZE: Hints the compiler to vectorize loops for SIMD.
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_VECTORIZE _Pragma("clang loop vectorize(enable)")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_VECTORIZE _Pragma("GCC optimize(\"tree-vectorize\")")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_VECTORIZE _Pragma("loop(ivdep)")
#else
    #define SENKAID_VECTORIZE
#endif

// Diagnostic control
// SENKAID_DIAGNOSTIC_PUSH/POP: Saves and restores warning state.
// SENKAID_IGNORE_WARNING(w): Suppresses specific warnings.
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
    #define SENKAID_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
    #define SENKAID_IGNORE_WARNING(w) _Pragma(SENKAID_STRINGIFY(clang diagnostic ignored w))
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
    #define SENKAID_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
    #define SENKAID_IGNORE_WARNING(w) _Pragma(SENKAID_STRINGIFY(GCC diagnostic ignored w))
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define SENKAID_DIAGNOSTIC_POP _Pragma("warning(pop)")
    #define SENKAID_IGNORE_WARNING(w) _Pragma(SENKAID_STRINGIFY(warning(disable: w)))
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_DIAGNOSTIC_PUSH _Pragma("warning push")
    #define SENKAID_DIAGNOSTIC_POP _Pragma("warning pop")
    #define SENKAID_IGNORE_WARNING(w) _Pragma(SENKAID_STRINGIFY(warning(disable: w)))
#else
    #define SENKAID_DIAGNOSTIC_PUSH
    #define SENKAID_DIAGNOSTIC_POP
    #define SENKAID_IGNORE_WARNING(w)
#endif

// Unused value (e.g. (void)x;)
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_UNUSED_VALUE SENKAID_IGNORE_WARNING("-Wunused-value")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_UNUSED_VALUE SENKAID_IGNORE_WARNING("-Wunused-value")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_UNUSED_VALUE SENKAID_IGNORE_WARNING(4555)
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_UNUSED_VALUE SENKAID_IGNORE_WARNING(177)
#else
    #define SENKAID_IGNORE_WARNING_UNUSED_VALUE
#endif

// Unused function
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_UNUSED_FUNCTION SENKAID_IGNORE_WARNING("-Wunused-function")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_UNUSED_FUNCTION SENKAID_IGNORE_WARNING("-Wunused-function")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_UNUSED_FUNCTION SENKAID_IGNORE_WARNING(4505)
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_UNUSED_FUNCTION SENKAID_IGNORE_WARNING(177)
#else
    #define SENKAID_IGNORE_WARNING_UNUSED_FUNCTION
#endif

// Unused variable
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_UNUSED_VARIABLE SENKAID_IGNORE_WARNING("-Wunused-variable")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_UNUSED_VARIABLE SENKAID_IGNORE_WARNING("-Wunused-variable")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_UNUSED_VARIABLE SENKAID_IGNORE_WARNING(4101)
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_UNUSED_VARIABLE SENKAID_IGNORE_WARNING(186)
#else
    #define SENKAID_IGNORE_WARNING_UNUSED_VARIABLE
#endif

// Unused parameter
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_UNUSED_PARAMETER SENKAID_IGNORE_WARNING("-Wunused-parameter")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_UNUSED_PARAMETER SENKAID_IGNORE_WARNING("-Wunused-parameter")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_UNUSED_PARAMETER SENKAID_IGNORE_WARNING(4100)
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_UNUSED_PARAMETER SENKAID_IGNORE_WARNING(869)
#else
    #define SENKAID_IGNORE_WARNING_UNUSED_PARAMETER
#endif

// Unreachable code
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_UNREACHABLE_CODE SENKAID_IGNORE_WARNING("-Wunreachable-code")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_UNREACHABLE_CODE SENKAID_IGNORE_WARNING("-Wunreachable-code")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_UNREACHABLE_CODE SENKAID_IGNORE_WARNING(4702)
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_UNREACHABLE_CODE SENKAID_IGNORE_WARNING(111)
#else
    #define SENKAID_IGNORE_WARNING_UNREACHABLE_CODE
#endif

// Fallthrough without attribute or comment
#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_IGNORE_WARNING_IMPLICIT_FALLTHROUGH SENKAID_IGNORE_WARNING("-Wimplicit-fallthrough")
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_IGNORE_WARNING_IMPLICIT_FALLTHROUGH SENKAID_IGNORE_WARNING("-Wimplicit-fallthrough")
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_IGNORE_WARNING_IMPLICIT_FALLTHROUGH /* MSVC does not warn about fallthrough */
#elif defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_IGNORE_WARNING_IMPLICIT_FALLTHROUGH /* Not available or suppressed through pragma diagnostic */
#else
    #define SENKAID_IGNORE_WARNING_IMPLICIT_FALLTHROUGH
#endif

// Stringification for debugging
// SENKAID_STRINGIFY: Converts a macro to a string.
// SENKAID_COMPILER_INFO: Provides compiler name and version as a string.
#define SENKAID_COMPILER_INFO SENKAID_COMPILER_NAME " " SENKAID_STRINGIFY(SENKAID_COMPILER_VERSION)

// Profile-guided optimization
// SENKAID_PGO_INSTRUMENT: Enables PGO instrumentation for performance-critical code.
// Requires CMake flag SENKAID_ENABLE_PGO.
#ifdef SENKAID_ENABLE_PGO
    #if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
        #define SENKAID_PGO_INSTRUMENT __attribute__((profile))
    #else
        #define SENKAID_PGO_INSTRUMENT
    #endif
#else
    #define SENKAID_PGO_INSTRUMENT
#endif

#ifdef SENKAID_COMPILER_NVCC
    #define SENKAID_CUDA_HOST_DEVICE __host__ __device__
    #define SENKAID_CUDA_FAST_MATH __attribute__((optimize("fast-math")))
#else
    #define SENKAID_CUDA_HOST_DEVICE
    #define SENKAID_CUDA_FAST_MATH
#endif

#if defined(__FMA__)
    #define SENKAID_HAS_FMA
#elif defined(__ARM_FEATURE_FMA)
    #define SENKAID_HAS_FMA
#endif

#if defined(SENKAID_HAS_IMMINTRIN_H) && (defined(__x86_64__) || defined(_M_X64))
    #define SENKAID_HAS_CPUID
#endif

#ifdef _OPENMP
    #define SENKAID_HAS_OPENMP
#endif

// ADDITIONAL MACROS
#if SENKAID_HAS_INCLUDE(<stacktrace>)
    #define SENKAID_HAS_STACKTRACE
    #include <string>
#endif
