#pragma once

/**
 * @file compiler.hpp
 * @brief Compiler and platform detection macros for the Senkaid library.
 *
 * Defines macros for identifying compilers, platforms, and C++ standards, along with
 * optimization and pragma utilities. Used to ensure portability and performance across
 * different environments.
 */

#include <cstdint>
#include "version.hpp"

// ==========================================
// Compiler Identification
// ==========================================

#if defined(__clang__)
    #define SENKAID_COMPILER_CLANG 1
    #define SENKAID_CLANG_VERSION_MAJOR __clang_major__
    #define SENKAID_CLANG_VERSION_MINOR __clang_minor__
    #define SENKAID_CLANG_VERSION_PATCH __clang_patchlevel__
    #define SENKAID_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #define SENKAID_COMPILER_AT_LEAST_CLANG(major, minor) \
        (SENKAID_COMPILER_VERSION >= (major * 10000 + minor * 100))
    #define SENKAID_COMPILER_NAME_STRING "Clang"
#elif defined(__GNUC__) || defined(__GNUG__)
    #define SENKAID_COMPILER_GCC 1
    #define SENKAID_GCC_VERSION_MAJOR __GNUC__
    #define SENKAID_GCC_VERSION_MINOR __GNUC_MINOR__
    #define SENKAID_GCC_VERSION_PATCH __GNUC_PATCHLEVEL__
    #define SENKAID_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #define SENKAID_COMPILER_AT_LEAST_GCC(major, minor) \
        (SENKAID_COMPILER_VERSION >= (major * 10000 + minor * 100))
    #define SENKAID_COMPILER_NAME_STRING "GCC"
#elif defined(_MSC_VER)
    #define SENKAID_COMPILER_MSVC 1
    #define SENKAID_MSVC_VERSION _MSC_VER
    #define SENKAID_COMPILER_VERSION _MSC_VER
    #define SENKAID_COMPILER_AT_LEAST_MSVC(major) \
        (SENKAID_COMPILER_VERSION >= major)
    #define SENKAID_COMPILER_NAME_STRING "MSVC"
#elif defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
    #define SENKAID_COMPILER_INTEL 1
    #define SENKAID_COMPILER_VERSION (__INTEL_COMPILER ? __INTEL_COMPILER : __INTEL_LLVM_COMPILER)
    #define SENKAID_COMPILER_AT_LEAST_INTEL(major) \
        (SENKAID_COMPILER_VERSION >= major)
    #define SENKAID_COMPILER_NAME_STRING "INTEL"
#else
    #warning "Unsupported compiler for senkaid; using default configuration"
    #define SENKAID_COMPILER_UNKNOWN 1
    #define SENKAID_COMPILER_VERSION 0
#endif

// ==========================================
// GPU and SYCL Compilers
// ==========================================

#if defined(__CUDACC__)
    #define SENKAID_COMPILER_NVCC 1
    #define SENKAID_COMPILER_NAME_STRING "CUDA"
#endif

#if defined(__HIPCC__)
    #define SENKAID_COMPILER_HIPCC 1
    #define SENKAID_COMPILER_NAME_STRING "HIPCC"
#endif

#if defined(__INTEL_LLVM_COMPILER) && defined(SYCL_LANGUAGE_VERSION)
    #define SENKAID_COMPILER_SYCL 1
    #define SENKAID_COMPILER_NAME_STRING "SYCL&Intel"
#endif

// ==========================================
// C++ Standard Detection
// ==========================================

#if defined(__cplusplus)
    #if __cplusplus >= 202302L
        #define SENKAID_CXX23 1
    #elif __cplusplus >= 202002L
        #define SENKAID_CXX20 1 
    #else
        #error "senkaid requires at least C++20"
    #endif
#else
    #error "C++ compiler required"
#endif

// ==========================================
// Helper Macros: Force Inline, Align, etc.
// ==========================================

#if defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_FORCE_INLINE __forceinline
    #define SENKAID_NO_INLINE __declspec(noinline)
    #define SENKAID_ALIGN(N) __declspec(align(N))
#elif defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_FORCE_INLINE inline __attribute__((always_inline))
    #define SENKAID_NO_INLINE __attribute__((noinline))
    #define SENKAID_ALIGN(N) __attribute__((aligned(N)))
#else
    #define SENKAID_FORCE_INLINE inline
    #define SENKAID_NO_INLINE
    #define SENKAID_ALIGN(N)
#endif

// ==========================================
// Pragma Helpers
// ==========================================

#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC)
    #define SENKAID_PRAGMA(x) _Pragma(#x)
    #define SENKAID_PRAGMA_WARNING_PUSH SENKAID_PRAGMA(GCC diagnostic push)
    #define SENKAID_PRAGMA_WARNING_POP SENKAID_PRAGMA(GCC diagnostic pop)
    #define SENKAID_PRAGMA_DISABLE_WARNING(warning) SENKAID_PRAGMA(GCC diagnostic ignored warning)
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_PRAGMA(x) __pragma(x)
    #define SENKAID_PRAGMA_WARNING_PUSH SENKAID_PRAGMA(warning(push))
    #define SENKAID_PRAGMA_WARNING_POP SENKAID_PRAGMA(warning(pop))
    #define SENKAID_PRAGMA_DISABLE_WARNING(warning) SENKAID_PRAGMA(warning(disable: warning))
#else
    #define SENKAID_PRAGMA(x)
    #define SENKAID_PRAGMA_WARNING_PUSH
    #define SENKAID_PRAGMA_WARNING_POP
    #define SENKAID_PRAGMA_DISABLE_WARNING(warning)
#endif
