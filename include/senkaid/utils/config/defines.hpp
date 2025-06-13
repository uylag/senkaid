#pragma once

/**
 * @file defines.hpp
 * @brief Global configuration macros and constants for the Senkaid library.
 *
 * Defines library-wide constants, version information, and feature toggles.
 * Includes platform and compiler settings from compiler.hpp and platform.hpp.
 */

#include "compiler.hpp"
#include "platform.hpp"
#include <cstdint>

// ==========================================
// Library Version
// ==========================================

#define SENKAID_VERSION_STRING senkaid::utils::config::version::_string
#define SENKAID_VERSION_STRING_FULL senkaid::utils::config::version::_full
#define SENKAID_VERSION_MAJOR senkaid::utils::config::version::_major
#define SENKAID_VERSION_MINOR senkaid::utils::config::version::_minor
#define SENKAID_VERSION_PATCH senkaid::utils::config::version::_patch
#define SENKAID_VERSION_AT_LEAST(major, minor, patch) senkaid::utils::config::version::at_least(major, minor, patch)
#define SENKAID_VERSION (SENKAID_VERSION_MAJOR * 10000 + SENKAID_VERSION_MINOR * 100 + SENKAID_VERSION_PATCH)


// ==========================================
// ABI Version
// ==========================================

#define SENKAID_ABI_VERSION 1

// ==========================================
// Feature Toggles
// ==========================================

#ifndef SENKAID_ENABLE_DEBUG
    #define SENKAID_ENABLE_DEBUG 0
#endif

#ifndef SENKAID_ENABLE_CUDA
    #define SENKAID_ENABLE_CUDA 0
#endif

#ifndef SENKAID_ENABLE_ROCM
    #define SENKAID_ENABLE_ROCM 0
#endif

#ifndef SENKAID_ENABLE_SYCL
    #define SENKAID_ENABLE_SYCL 0
#endif

#ifndef SENKAID_ENABLE_OPENMP
    #define SENKAID_ENABLE_OPENMP 0
#endif

// ==========================================
// Optimization Settings
// ==========================================

namespace senkaid::config {
    // Default matrix storage order
    inline constexpr bool is_row_major = true;

    // Default unroll factor for loops
    inline constexpr std::size_t loop_unroll_factor = 4;

    // Default tile size for matrix operations
    inline constexpr std::size_t tile_size = 64;

    // Floating-point epsilon for comparisons
    template<typename T>
    inline constexpr T epsilon = T{1e-10};

    // Maximum dimension for static matrices
    inline constexpr std::size_t max_static_dim = 16;
} // namespace senkaid::config

// ==========================================
// Library-Wide Constants
// ==========================================

#define SENKAID_DEFAULT_ALIGNMENT senkaid::config::simd_alignment

// ==========================================
// Error Handling
// ==========================================

#if SENKAID_ENABLE_DEBUG
    #define SENKAID_ASSERTIONS_ENABLED 1
#else
    #define SENKAID_ASSERTIONS_ENABLED 0
#endif

// ==========================================
// Deprecated Features
// ==========================================

#if defined(SENKAID_CXX20) || defined(SENKAID_CXX23)
    #define SENKAID_DEPRECATED [[deprecated]]
#else
    #define SENKAID_DEPRECATED
#endif
