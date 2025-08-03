#pragma once

// flags.hpp: Debugging and diagnostic configuration flags for the senkaid library.
// Provides toggles for controlling assertions, logging, tracing, and profiling, surpassing Eigen’s limited debugging facilities.
// Uses macros from compiler.hpp for compiler detection and debugging support.

// Debug mode control
// SENKAID_DEBUG_ENABLED: Enables debugging features in debug builds, disabled in release builds (NDEBUG).
#ifdef NDEBUG
    #define SENKAID_DEBUG_ENABLED 0
#else
    #define SENKAID_DEBUG_ENABLED 1
#endif

// Debugging verbosity levels (per <REQUIREMENTS> for usability)
// SENKAID_DEBUG_LEVEL: Controls debugging verbosity (0: off, 1: minimal, 2: detailed, 3: verbose).
#ifndef SENKAID_DEBUG_LEVEL
    #define SENKAID_DEBUG_LEVEL 2
#endif

// Assertion control (integrates with utils/debug/assert.hpp)
// SENKAID_ASSERT_ENABLED: Enables assertions, controlled by NDEBUG and debug level.
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 1
    #define SENKAID_ASSERT_ENABLED 1
#else
    #define SENKAID_ASSERT_ENABLED 0
#endif

// Logging control (integrates with utils/debug/logger.hpp)
// SENKAID_LOG_ENABLED: Enables logging output for debugging.
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 1
    #define SENKAID_LOG_ENABLED 1
#else
    #define SENKAID_LOG_ENABLED 0
#endif

// SENKAID_LOG_LEVEL: Controls logging verbosity (0: off, 1: errors, 2: warnings, 3: info).
#ifndef SENKAID_LOG_LEVEL
    #define SENKAID_LOG_LEVEL SENKAID_DEBUG_LEVEL
#endif

// Tracing control (integrates with utils/debug/trace.hpp)
// SENKAID_TRACE_ENABLED: Enables stack tracing for detailed diagnostics.
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 2 && defined(SENKAID_HAS_STACKTRACE)
    #define SENKAID_TRACE_ENABLED 1
#else
    #define SENKAID_TRACE_ENABLED 0
#endif

// Profiling control
// SENKAID_PROFILE_ENABLED: Enables performance profiling for hot paths (e.g., matrix/tensor operations).
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 3
    #define SENKAID_PROFILE_ENABLED 1
#else
    #define SENKAID_PROFILE_ENABLED 0
#endif

// Memory debugging (integrates with utils/memory/tracker.hpp)
// SENKAID_MEMORY_DEBUG_ENABLED: Enables memory allocation tracking for leak detection.
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 2
    #define SENKAID_MEMORY_DEBUG_ENABLED 1
#else
    #define SENKAID_MEMORY_DEBUG_ENABLED 0
#endif

// Bounds checking (integrates with core/matrix, core/tensor)
// SENKAID_BOUNDS_CHECK_ENABLED: Enables bounds checking for matrix/tensor indices.
#if SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 2
    #define SENKAID_BOUNDS_CHECK_ENABLED 1
#else
    #define SENKAID_BOUNDS_CHECK_ENABLED 0
#endif

// CUDA debugging (integrates with backend/cuda)
// SENKAID_CUDA_DEBUG_ENABLED: Enables CUDA-specific debugging (e.g., device-side assertions).
#if defined(SENKAID_COMPILER_NVCC) && SENKAID_DEBUG_ENABLED && SENKAID_DEBUG_LEVEL >= 1
    #define SENKAID_CUDA_DEBUG_ENABLED 1
#else
    #define SENKAID_CUDA_DEBUG_ENABLED 0
#endif

// Warning suppression for debugging code
#if SENKAID_DEBUG_ENABLED
    #define SENKAID_DEBUG_DIAGNOSTIC_START \
        SENKAID_DIAGNOSTIC_PUSH \
        SENKAID_IGNORE_WARNING_UNUSED_VALUE \
        SENKAID_IGNORE_WARNING_UNUSED_VARIABLE
    #define SENKAID_DEBUG_DIAGNOSTIC_END SENKAID_DIAGNOSTIC_POP
#else
    #define SENKAID_DEBUG_DIAGNOSTIC_START
    #define SENKAID_DEBUG_DIAGNOSTIC_END
#endif

// Conditional debug code execution
// SENKAID_DEBUG_ONLY: Executes code only in debug builds to avoid performance overhead.
#if SENKAID_DEBUG_ENABLED
    #define SENKAID_DEBUG_ONLY(code) code
#else
    #define SENKAID_DEBUG_ONLY(code)
#endif

// Compile-time debug flag check
// SENKAID_STATIC_DEBUG_CHECK: Compile-time assertion for debug configuration.
#if defined(SENKAID_HAS_CONSTEVAL)
    #define SENKAID_STATIC_DEBUG_CHECK(condition, message) \
        do { \
            consteval { \
                if (!(condition)) { \
                    static_assert(false, message); \
                } \
            } \
        } while (0)
#else
    #define SENKAID_STATIC_DEBUG_CHECK(condition, message) \
        static_assert(condition, message)
#endif
