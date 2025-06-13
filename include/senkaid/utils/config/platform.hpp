#pragma once

/**
 * @file platform.hpp
 * @brief Platform and architecture detection macros for the Senkaid library.
 *
 * Defines macros for identifying operating systems, CPU architectures, and platform-specific
 * configurations. Used to ensure portability and performance across different environments.
 * Complements utils/config/compiler.hpp by providing platform-specific settings.
 */

#include <cstdint>

// ==========================================
// Platform Identification
// ==========================================

#if defined(_WIN32)
    #define SENKAID_PLATFORM_WINDOWS 1
    #define SENKAID_PLATFORM_NAME_STRING "Windows"
#elif defined(__linux__)
    #define SENKAID_PLATFORM_LINUX 1
    #define SENKAID_PLATFORM_NAME_STRING "Linux"
#elif defined(__APPLE__)
    #define SENKAID_PLATFORM_MACOS 1
    #define SENKAID_PLATFORM_NAME_STRING "MacOS"
#else
    #define SENKAID_PLATFORM_UNKNOWN 1
    #define SENKAID_PLATFORM_NAME_STRING "unknown"
    #warning "Unsupported platform for senkaid; using default configuration"
#endif

// ==========================================
// Architecture Identification
// ==========================================

#if defined(__x86_64__) || defined(_M_X64)
    #define SENKAID_ARCH_X86_64 1
    #define SENKAID_ARCH_AARCH64 0
    #define SENKAID_ARCH_UNKNOWN 0
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define SENKAID_ARCH_AARCH64 1
    #define SENKAID_ARCH_X86_64 0
    #define SENKAID_ARCH_UNKNOWN 0
#else
    #define SENKAID_ARCH_UNKNOWN 1
    #define SENKAID_ARCH_X86_64 0
    #define SENKAID_ARCH_AARCH64 0
    #warning "Unsupported architecture for senkaid; using default configuration"
#endif

// ==========================================
// Platform-Specific Constants
// ==========================================

namespace senkaid::utils::config {
    // Cache line size for memory alignment (architecture-dependent)
    #if defined(SENKAID_ARCH_X86_64)
        inline constexpr std::size_t cache_line_size = 64; // Common for x86_64
    #elif defined(SENKAID_ARCH_AARCH64)
        inline constexpr std::size_t cache_line_size = 64; // Common for AArch64
    #else
        inline constexpr std::size_t cache_line_size = 64; // Default fallback
    #endif

    // SIMD alignment (architecture- and compiler-dependent)
    #if defined(SENKAID_ARCH_X86_64) && (defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC))
        inline constexpr std::size_t simd_alignment = 32; // AVX alignment
    #elif defined(SENKAID_ARCH_AARCH64)
        inline constexpr std::size_t simd_alignment = 16; // NEON alignment
    #else
        inline constexpr std::size_t simd_alignment = 16; // Conservative default
    #endif

    // Page size for memory allocation (platform-dependent)
    #if defined(SENKAID_PLATFORM_WINDOWS)
        inline constexpr std::size_t page_size = 4096; // Windows default
    #elif defined(SENKAID_PLATFORM_LINUX) || defined(SENKAID_PLATFORM_MACOS)
        inline constexpr std::size_t page_size = 4096; // POSIX default
    #else
        inline constexpr std::size_t page_size = 4096; // Fallback
    #endif
} // namespace senkaid::config

// ==========================================
// Platform-Specific Features
// ==========================================

#if defined(SENKAID_PLATFORM_LINUX)
    #define SENKAID_HAS_POSIX 1
    #define SENKAID_HAS_MMAP 1
#elif defined(SENKAID_PLATFORM_MACOS)
    #define SENKAID_HAS_POSIX 1
    #define SENKAID_HAS_MMAP 1
#else
    #define SENKAID_HAS_POSIX 0
    #define SENKAID_HAS_MMAP 0
#endif

#if defined(SENKAID_ARCH_X86_64)
    #define SENKAID_HAS_AVX 1 // Assume AVX support; refine in backend/simd
#else
    #define SENKAID_HAS_AVX 0
#endif

// ==========================================
// Platform-Specific Path Handling
// ==========================================

#if defined(SENKAID_PLATFORM_WINDOWS)
    #define SENKAID_PATH_SEPARATOR "\\"
#else
    #define SENKAID_PATH_SEPARATOR "/"
#endif
