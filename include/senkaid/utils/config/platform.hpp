#pragma once

#include <unistd.h>

// platform.hpp: Defines platform-specific macros for the senkaid library.
// Detects operating system, architecture, and platform-specific features to ensure portability and performance.
// All macros are prefixed with SENKAID_ to avoid conflicts and can be overridden via CMake.

// Operating system detection
// SENKAID_PLATFORM_LINUX: Defined for Linux-based systems.
// SENKAID_PLATFORM_WINDOWS: Defined for Windows systems (32-bit or 64-bit).
// SENKAID_PLATFORM_MACOS: Defined for macOS systems.
// SENKAID_PLATFORM_UNKNOWN: Fallback for unsupported platforms.
#if defined(__linux__)
    #define SENKAID_PLATFORM_LINUX
#elif defined(_WIN32)
    #define SENKAID_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define SENKAID_PLATFORM_MACOS
#else
    #define SENKAID_PLATFORM_UNKNOWN
#endif

// Architecture detection
// SENKAID_ARCH_X86_64: Defined for 64-bit x86 architectures.
// SENKAID_ARCH_X86: Defined for 32-bit x86 architectures.
// SENKAID_ARCH_AARCH64: Defined for 64-bit ARM architectures.
// SENKAID_ARCH_ARM: Defined for 32-bit ARM architectures.
// SENKAID_ARCH_UNKNOWN: Fallback for unsupported architectures.
#if defined(__x86_64__) || defined(_M_X64)
    #define SENKAID_ARCH_X86_64
#elif defined(__i386__) || defined(_M_IX86)
    #define SENKAID_ARCH_X86
#elif defined(__aarch64__)
    #define SENKAID_ARCH_AARCH64
#elif defined(__arm__) || defined(_M_ARM)
    #define SENKAID_ARCH_ARM
#else
    #define SENKAID_ARCH_UNKNOWN
#endif

// POSIX compliance
// SENKAID_POSIX_VERSION: Defines the POSIX version for Linux/macOS systems.
// Used for POSIX-specific features (e.g., mmap, pthreads).
#if defined(SENKAID_PLATFORM_LINUX) || defined(SENKAID_PLATFORM_MACOS)
    #ifdef _POSIX_VERSION
        #define SENKAID_POSIX_VERSION _POSIX_VERSION
    #else
        #define SENKAID_POSIX_VERSION 0
    #endif
#else
    #define SENKAID_POSIX_VERSION 0
#endif

// Platform-specific memory management
// SENKAID_HAS_ALIGNED_ALLOC: Indicates availability of aligned_alloc (POSIX) or _aligned_malloc (Windows).
#if defined(SENKAID_PLATFORM_LINUX) || defined(SENKAID_PLATFORM_MACOS)
    #if SENKAID_POSIX_VERSION >= 200112L
        #define SENKAID_HAS_ALIGNED_ALLOC
    #endif
#elif defined(SENKAID_PLATFORM_WINDOWS)
    #define SENKAID_HAS_ALIGNED_ALLOC
#endif

// Platform-specific file I/O
// SENKAID_HAS_POSIX_IO: Indicates availability of POSIX file I/O (e.g., open, read).
#if defined(SENKAID_PLATFORM_LINUX) || defined(SENKAID_PLATFORM_MACOS)
    #if SENKAID_POSIX_VERSION >= 199309L
        #define SENKAID_HAS_POSIX_IO
    #endif
#endif

// Platform-specific SIMD support
// Relies on compiler.hpp for SIMD detection (e.g., SENKAID_HAS_AVX2, SENKAID_HAS_NEON).
// SENKAID_HAS_PLATFORM_SIMD: Indicates platform supports SIMD instructions.
#if defined(SENKAID_ARCH_X86_64) || defined(SENKAID_ARCH_X86)
    #if defined(SENKAID_HAS_SSE) || defined(SENKAID_HAS_SSE2) || defined(SENKAID_HAS_SSE4_2) || \
        defined(SENKAID_HAS_AVX) || defined(SENKAID_HAS_AVX2) || defined(SENKAID_HAS_AVX512)
        #define SENKAID_HAS_PLATFORM_SIMD
    #endif
#elif defined(SENKAID_ARCH_AARCH64) || defined(SENKAID_ARCH_ARM)
    #if defined(SENKAID_HAS_NEON)
        #define SENKAID_HAS_PLATFORM_SIMD
    #endif
#endif

// Platform-specific GPU support
// SENKAID_HAS_CUDA: Indicates availability of CUDA runtime.
// SENKAID_HAS_ROCM: Indicates availability of ROCm/HIP runtime.
#if __has_include(<cuda_runtime.h>) && defined(SENKAID_ENABLE_CUDA)
    #define SENKAID_HAS_CUDA
#endif
#if __has_include(<hip/hip_runtime.h>) && defined(SENKAID_ENABLE_ROCM)
    #define SENKAID_HAS_ROCM
#endif

// SENKAID_HAS_SYCL: Indicates availability of SYCL runtime.
#if __has_include(<sycl/sycl.hpp>) && defined(SENKAID_ENABLE_SYCL)
    #define SENKAID_HAS_SYCL
#endif

// Platform-specific optimization flags
// SENKAID_PLATFORM_OPTIMIZE: Enables platform-specific optimizations (e.g., -march=native).
// Configurable via CMake to avoid incompatible instructions.
#ifndef SENKAID_PLATFORM_OPTIMIZE
    #define SENKAID_PLATFORM_OPTIMIZE 0
#endif

// SENKAID_PLATFORM_CACHE_LINE: Defines the cache line size for the platform.
// Default: 64 bytes for most modern architectures (x86_64, AArch64).
#ifndef SENKAID_PLATFORM_CACHE_LINE
    #define SENKAID_PLATFORM_CACHE_LINE 64
#endif

// Platform-specific threading configuration
// SENKAID_DEFAULT_THREAD_COUNT: Default number of threads for parallel operations.
// Default: 0 (auto-detect via hardware_concurrency or environment variable).
#ifndef SENKAID_DEFAULT_THREAD_COUNT
    #define SENKAID_DEFAULT_THREAD_COUNT 0
#endif

// CPU Feature Detection Support
// SENKAID_HAS_CPUID: Indicates if runtime CPU feature detection is possible/supported.
// Currently defined for x86/x86_64 in compiler.hpp.
// For ARM, runtime detection is possible via different means (e.g., getauxval on Linux).
// This macro indicates general platform support for such checks.
#if defined(SENKAID_ARCH_X86_64) || defined(SENKAID_ARCH_X86)
    #ifndef SENKAID_HAS_CPUID
        #define SENKAID_HAS_CPUID
    #endif
#elif defined(SENKAID_ARCH_AARCH64) || defined(SENKAID_ARCH_ARM)
    // For example, on Linux, check for `getauxval` availability.
    #if defined(SENKAID_PLATFORM_LINUX) && defined(SENKAID_HAS_CXX_THREAD) // Heuristic: implies basic system headers
        #define SENKAID_HAS_CPUID // Or a more specific check if needed
    #endif
#endif
