#pragma once

// assert.hpp: Assertion and debugging utilities for the senkaid library.
// Provides customizable runtime and compile-time assertions, surpassing Eigen’s basic assert mechanism.
// Uses macros from compiler.hpp for compiler detection and debugging support.

#include <cassert>
#include <senkaid/utils/config/root.hpp>

// Conditional includes for portability
#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<stdexcept>)
        #include <stdexcept>
    #endif
    #if SENKAID_HAS_INCLUDE(<cstdio>)
        #include <cstdio>
    #endif
    #if SENKAID_HAS_INCLUDE(<cstddef>)
        #include <cstddef>
    #endif
    #if defined(SENKAID_HAS_STACKTRACE)
        #include <stacktrace>
    #endif
#else
    #include <stdexcept>
    #include <cstdio>
    #include <cstddef>
#endif


// Debug mode control
// SENKAID_ASSERT_ENABLED: Enables assertions in debug builds, disabled in release builds (NDEBUG).
#ifdef NDEBUG
    #define SENKAID_ASSERT_ENABLED 0
#else
    #define SENKAID_ASSERT_ENABLED 1
#endif

// Assertion levels for granularity (per <REQUIREMENTS> for usability)
// SENKAID_ASSERT_LEVEL: Controls assertion verbosity (0: off, 1: critical, 2: all).
#ifndef SENKAID_ASSERT_LEVEL
    #define SENKAID_ASSERT_LEVEL 2
#endif

// Compile-time assertion
// SENKAID_STATIC_ASSERT: Compile-time check using static_assert, leveraging C++20 consteval if available.
#if defined(SENKAID_HAS_CONSTEVAL)
    #define SENKAID_STATIC_ASSERT(condition, message) \
        do { \
            consteval { \
                if (!(condition)) { \
                    static_assert(false, message); \
                } \
            } \
        } while (0)
#else
    #define SENKAID_STATIC_ASSERT(condition, message) \
        static_assert(condition, message)
#endif

// Runtime assertion macros
// SENKAID_ASSERT: General-purpose assertion with message and source location.
#if SENKAID_ASSERT_ENABLED && SENKAID_ASSERT_LEVEL >= 1
    #define SENKAID_ASSERT(condition, message) assert((condition) && message)
#else
    #define SENKAID_ASSERT(condition, message)
#endif

// SENKAID_ASSERT_CRITICAL: Critical assertion for invariants, always enabled unless disabled.
#ifndef SENKAID_NO_CRITICAL_ASSERTS
    #define SENKAID_ASSERT_CRITICAL(condition, message) \
        do { \
            SENKAID_DIAGNOSTIC_PUSH \
            SENKAID_IGNORE_WARNING_UNUSED_VALUE \
            if (SENKAID_UNLIKELY(!(condition))) { \
                ::senkaid::detail::assert_fail( \
                    #condition, \
                    message, \
                    __FILE__, \
                    __LINE__, \
                    SENKAID_COMPILER_INFO \
                ); \
            } \
            SENKAID_DIAGNOSTIC_POP \
        } while (0)
#else
    #define SENKAID_ASSERT_CRITICAL(condition, message)
#endif

// SENKAID_ASSERT_DEBUG: Debug-only assertions for non-critical checks.
#if SENKAID_ASSERT_ENABLED && SENKAID_ASSERT_LEVEL >= 2
    #define SENKAID_ASSERT_DEBUG(condition, message) SENKAID_ASSERT(condition, message)
#else
    #define SENKAID_ASSERT_DEBUG(condition, message)
#endif

// SENKAID_ASSERT_BOUNDS: Specialized for bounds checking (e.g., matrix/tensor indices).
#if SENKAID_ASSERT_ENABLED && SENKAID_ASSERT_LEVEL >= 2
    #define SENKAID_ASSERT_BOUNDS(index, size, message) \
        SENKAID_ASSERT( \
            static_cast<std::size_t>(index) < static_cast<std::size_t>(size), \
            message \
        )
#else
    #define SENKAID_ASSERT_BOUNDS(index, size, message)
#endif

// SENKAID_ASSERT_NOT_NULL: Specialized for null pointer checks.
#if SENKAID_ASSERT_ENABLED && SENKAID_ASSERT_LEVEL >= 2
    #define SENKAID_ASSERT_NOT_NULL(ptr, message) \
        SENKAID_ASSERT((ptr) != nullptr, message)
#else
    #define SENKAID_ASSERT_NOT_NULL(ptr, message)
#endif

// CUDA-specific assertion for __device__ code
#if defined(SENKAID_COMPILER_NVCC)
    #define SENKAID_CUDA_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                printf("[senkaid] CUDA Assertion failed: %s\nCondition: %s\nFile: %s:%d\n", \
                    message ? message : "No message provided", #condition, __FILE__, __LINE__); \
                assert(false); \
            } \
        } while (0)
#else
    #define SENKAID_CUDA_ASSERT(condition, message)
#endif

namespace senkaid::detail {
// SENKAID_NORETURN function to handle assertion failures with detailed error reporting.
SENKAID_NORETURN SENKAID_FORCE_INLINE void assert_fail(
    const char* condition,
    const char* message,
    const char* file,
    int line,
    const char* compiler_info
) SENKAID_CUDA_HOST_DEVICE {
    // Buffer to format error message
    char buffer[1024];
    #if defined(SENKAID_COMPILER_MSVC) && !defined(SENKAID_COMPILER_NVCC)
        _snprintf_s(
            buffer,
            sizeof(buffer),
            _TRUNCATE,
            "[senkaid] Assertion failed: %s\nCondition: %s\nFile: %s:%d\nCompiler: %s",
            message ? message : "No message provided",
            condition,
            file,
            line,
            compiler_info
        );
    #else
        snprintf(
            buffer,
            sizeof(buffer),
            "[senkaid] Assertion failed: %s\nCondition: %s\nFile: %s:%d\nCompiler: %s",
            message ? message : "No message provided",
            condition,
            file,
            line,
            compiler_info
        );
    #endif

    std::fputs(buffer, stderr);
    std::abort();

//    #if defined(SENKAID_HAS_STACKTRACE) && !defined(SENKAID_COMPILER_NVCC)
//        // Append stack trace if available (not supported in CUDA device code)
//        std::string stack_trace = std::to_string(std::stacktrace::current());
//        throw std::runtime_error(std::string(buffer) + "\nStacktrace:\n" + stack_trace);
//    #else
//        throw std::runtime_error(buffer);
//    #endif
}
} // namespace senkaid::detail
