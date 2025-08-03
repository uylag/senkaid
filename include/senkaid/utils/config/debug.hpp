#pragma once

// debug.hpp: Defines macros for debugging, logging, and runtime checks in the senkaid library.
// Provides assertions, numerical validation, logging, and profiling support.
// Self-contained with no dependencies on other utils/config/*.hpp files; all macros have defaults.

// --- Debug and Assertion Macros ---

// SENKAID_STRINGIFY: Utility macro for stringifying values.
#ifndef SENKAID_STRINGIFY
    #define SENKAID_STRINGIFY(x) #x
#endif

// SENKAID_DEBUG: Enables debug mode (assertions, logging, etc.).
// Default: Enabled if NDEBUG is not defined, disabled in release builds.
#ifndef SENKAID_DEBUG
    #ifdef NDEBUG
        #define SENKAID_DEBUG 0
    #else
        #define SENKAID_DEBUG 1
    #endif
#endif

// SENKAID_ASSERT: Custom assertion macro for runtime checks.
// Enabled in debug mode, no-op in release mode.
#if SENKAID_DEBUG
    #include <cassert>
    #define SENKAID_ASSERT(condition, message) assert((condition) && message)
#else
    #define SENKAID_ASSERT(condition, message)
#endif

// SENKAID_STATIC_ASSERT: Compile-time assertion using static_assert.
// Enabled regardless of debug mode for type safety and constraints.
#if defined(__cpp_static_assert) && __cpp_static_assert >= 201411L
    #define SENKAID_STATIC_ASSERT(condition, message) static_assert(condition, message)
#else
    #define SENKAID_STATIC_ASSERT(condition, message)
#endif

// --- Numerical Validation Macros ---

// SENKAID_VERIFY_NUMERICS: Enables runtime checks for NaN/Inf in numerical operations.
// Default: Enabled in debug mode, disabled in release for performance.
#ifndef SENKAID_VERIFY_NUMERICS
    #if SENKAID_DEBUG
        #define SENKAID_VERIFY_NUMERICS 1
    #else
        #define SENKAID_VERIFY_NUMERICS 0
    #endif
#endif

// SENKAID_CHECK_NAN_INF: Checks for NaN/Inf in a value and triggers an assertion if found.
#if SENKAID_VERIFY_NUMERICS
    #include <cmath>
    #define SENKAID_CHECK_NAN_INF(value, name) \
        SENKAID_ASSERT(!std::isnan(value) && !std::isinf(value), "NaN or Inf detected in " name)
#else
    #define SENKAID_CHECK_NAN_INF(value, name)
#endif

// SENKAID_USE_INTERVAL: Enables interval arithmetic for numerical validation.
// Default: Disabled unless explicitly enabled (e.g., via CMake).
#ifndef SENKAID_USE_INTERVAL
    #define SENKAID_USE_INTERVAL 0
#endif

// SENKAID_CHECK_INTERVAL: Validates a value within a specified interval [min, max].
#if SENKAID_USE_INTERVAL
    #define SENKAID_CHECK_INTERVAL(value, min, max, name) \
        SENKAID_ASSERT((value) >= (min) && (value) <= (max), "Value out of range in " name)
#else
    #define SENKAID_CHECK_INTERVAL(value, min, max, name)
#endif

// --- Logging Macros ---

// SENKAID_LOG_LEVEL: Defines logging verbosity (0 = off, 1 = error, 2 = warning, 3 = info, 4 = debug).
// Default: Disabled in release builds, debug level in debug builds.
#ifndef SENKAID_LOG_LEVEL
    #if SENKAID_DEBUG
        #define SENKAID_LOG_LEVEL 4
    #else
        #define SENKAID_LOG_LEVEL 0
    #endif
#endif

// SENKAID_LOG: Generic logging macro with level-based filtering.
#if SENKAID_LOG_LEVEL > 0
    #include <iostream>
    #define SENKAID_LOG(level, message) \
        do { \
            if ((level) <= SENKAID_LOG_LEVEL) { \
                std::cerr << "[SENKAID:" #level "] " << message << std::endl; \
            } \
        } while (0)
#else
    #define SENKAID_LOG(level, message)
#endif

// Convenience logging macros for specific levels
#define SENKAID_LOG_ERROR(message) SENKAID_LOG(1, message)
#define SENKAID_LOG_WARNING(message) SENKAID_LOG(2, message)
#define SENKAID_LOG_INFO(message) SENKAID_LOG(3, message)
#define SENKAID_LOG_DEBUG(message) SENKAID_LOG(4, message)

// --- Profiling Macros ---

// SENKAID_PROFILE: Enables profiling hooks for performance analysis (e.g., perf, Tracy).
// Default: Disabled unless explicitly enabled (e.g., via CMake).
#ifndef SENKAID_PROFILE
    #define SENKAID_PROFILE 0
#endif

// SENKAID_PROFILE_BEGIN/END: Marks a profiling scope for performance-critical sections.
#if SENKAID_PROFILE
    #define SENKAID_PROFILE_BEGIN(name) \
        do { \
            /* Hook for external profiler (e.g., Tracy, perf) */ \
        } while (0)
    #define SENKAID_PROFILE_END() \
        do { \
            /* End profiling scope */ \
        } while (0)
#else
    #define SENKAID_PROFILE_BEGIN(name)
    #define SENKAID_PROFILE_END()
#endif

// SENKAID_PROFILE_SCOPE: Convenience macro for profiling a named scope.
#define SENKAID_PROFILE_SCOPE(name) \
    SENKAID_PROFILE_BEGIN(name); \
    struct SENKAID_PROFILE_STRUCT_##name { \
        ~SENKAID_PROFILE_STRUCT_##name() { SENKAID_PROFILE_END(); } \
    } SENKAID_PROFILE_VAR_##name

// --- Debugging Utilities ---

// SENKAID_DEBUG_BREAK: Triggers a debugger breakpoint in debug mode.
#if SENKAID_DEBUG
    #if defined(_MSC_VER)
        #define SENKAID_DEBUG_BREAK() __debugbreak()
    #elif defined(__GNUC__) || defined(__clang__)
        #define SENKAID_DEBUG_BREAK() __builtin_trap()
    #else
        #define SENKAID_DEBUG_BREAK()
    #endif
#else
    #define SENKAID_DEBUG_BREAK()
#endif

// SENKAID_TRACE: Logs a trace message with file and line information in debug mode.
#if SENKAID_DEBUG
    #define SENKAID_TRACE(message) \
        SENKAID_LOG_DEBUG(__FILE__ ":" SENKAID_STRINGIFY(__LINE__) ": " message)
#else
    #define SENKAID_TRACE(message)
#endif
