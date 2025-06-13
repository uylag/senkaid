#pragma once

/**
 * @file debug.hpp
 * @brief Debugging and logging configuration macros for the Senkaid library.
 *
 * Defines macros and utilities for controlling debug assertions, logging levels,
 * and runtime checks. Integrates with compiler.hpp, platform.hpp, defines.hpp,
 * and config_macros.hpp to provide consistent debugging behavior.
 */

#include "compiler.hpp"
#include "platform.hpp"
#include "defines.hpp"
#include "config_macros.hpp"
#include <source_location>
#include <cstdint>
#include <cmath>

// ==========================================
// Debug Assertion Configuration
// ==========================================

#if SENKAID_ASSERTIONS_ENABLED
    #define SENKAID_DEBUG_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                senkaid::utils::config::debug::assert_failure(#condition, message, \
                    std::source_location::current()); \
            } \
        } while (false)
#else
    #define SENKAID_DEBUG_ASSERT(condition, message)
#endif

// Runtime check (always enabled)
#define SENKAID_RUNTIME_CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            senkaid::utils::config::debug::runtime_failure(#condition, message, \
                std::source_location::current()); \
        } \
    } while (false)

// Numerical stability check (controlled by SENKAID_CHECK_NUMERICS)
#if SENKAID_CHECK_NUMERICS
    #define SENKAID_NUMERIC_CHECK(value, message) \
        senkaid::utils::config::debug::numeric_check(value, #value, message, \
            std::source_location::current())
#else
    #define SENKAID_NUMERIC_CHECK(value, message)
#endif

// ==========================================
// Logging Configuration
// ==========================================

#define SENKAID_LOG_ERROR(message) \
    SENKAID_LOG_LEVEL_CHECK(1, senkaid::utils::config::debug::log_error, message)

#define SENKAID_LOG_WARNING(message) \
    SENKAID_LOG_LEVEL_CHECK(2, senkaid::utils::config::debug::log_warning, message)

#define SENKAID_LOG_INFO(message) \
    SENKAID_LOG_LEVEL_CHECK(3, senkaid::utils::config::debug::log_info, message)

// Internal logging level check
#define SENKAID_LOG_LEVEL_CHECK(level, func, message) \
    do { \
        if (SENKAID_LOG_LEVEL >= level) { \
            func(message, std::source_location::current()); \
        } \
    } while (false)

// ==========================================
// Debug Namespace
// ==========================================

namespace senkaid::utils::config::debug {
    // Logging severity levels
    enum class LogLevel : std::uint8_t {
        None = 0,
        Error = 1,
        Warning = 2,
        Info = 3
    };

    // Current log level (compile-time)
    inline constexpr LogLevel log_level = static_cast<LogLevel>(SENKAID_LOG_LEVEL);

    // Assertion failure handler
    [[noreturn]] SENKAID_FORCE_INLINE void assert_failure(
        const char* condition,
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Implementation in debug_impl.cpp or inline for header-only
        // Example: Print condition, message, file, line, then abort
    }

    // Runtime failure handler
    [[noreturn]] SENKAID_FORCE_INLINE void runtime_failure(
        const char* condition,
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Example: Throw std::runtime_error with formatted message
    }

    // Numeric stability check
    template<typename T>
    SENKAID_FORCE_INLINE void numeric_check(
        T value,
        const char* expr,
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Example: Check for NaN/Inf, log error if invalid
        if (!std::isfinite(value)) {
            runtime_failure(expr, message, loc);
        }
    }

    // Logging functions
    SENKAID_FORCE_INLINE void log_error(
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Example: Write to stderr with [ERROR] prefix
    }

    SENKAID_FORCE_INLINE void log_warning(
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Example: Write to stderr with [WARNING] prefix
    }

    SENKAID_FORCE_INLINE void log_info(
        const char* message,
        std::source_location loc = std::source_location::current()) {
        // Example: Write to stdout with [INFO] prefix
    }

    // Enable/disable logging at runtime (optional)
    inline bool logging_enabled = SENKAID_LOG_LEVEL > 0;

    // Set runtime log level (thread-safe if needed)
    SENKAID_FORCE_INLINE void set_log_level(LogLevel level) {
        logging_enabled = static_cast<std::uint8_t>(level) <= SENKAID_LOG_LEVEL;
    }
} // namespace senkaid::utils::config::debug

// ==========================================
// Profiling Configuration
// ==========================================

#if SENKAID_ENABLE_PROFILING
    #define SENKAID_PROFILE_SCOPE(name) \
        senkaid::utils::config::debug::ProfileScope SENKAID_CONCAT(profile_scope_, __LINE__)(name)
    #define SENKAID_PROFILE_FUNCTION() \
        SENKAID_PROFILE_SCOPE(__FUNCTION__)
#else
    #define SENKAID_PROFILE_SCOPE(name)
    #define SENKAID_PROFILE_FUNCTION()
#endif

// Macro for concatenating tokens
#define SENKAID_CONCAT(a, b) SENKAID_CONCAT_INNER(a, b)
#define SENKAID_CONCAT_INNER(a, b) a##b

// ==========================================
// Profiling Utilities
// ==========================================

namespace senkaid::utils::config::debug {
    // Profiling scope class
    class ProfileScope {
        const char* name_;
    public:
        SENKAID_FORCE_INLINE explicit ProfileScope(const char* name) : name_(name) {
            // Start timing (e.g., std::chrono or platform-specific)
        }
        SENKAID_FORCE_INLINE ~ProfileScope() {
            // End timing, log or store result
        }
        ProfileScope(const ProfileScope&) = delete;
        ProfileScope& operator=(const ProfileScope&) = delete;
    };

    // Platform-specific profiling initialization
    #if defined(SENKAID_PROFILE_PERF)
        SENKAID_FORCE_INLINE void init_profiler() {
            // Initialize Linux perf profiler
        }
    #elif defined(SENKAID_PROFILE_VTUNE)
        SENKAID_FORCE_INLINE void init_profiler() {
            // Initialize Intel VTune profiler
        }
    #else
        SENKAID_FORCE_INLINE void init_profiler() {}
    #endif

    // Start/stop profiling session
    SENKAID_FORCE_INLINE void start_profiling() {
        if (SENKAID_ENABLE_PROFILING) init_profiler();
    }

    SENKAID_FORCE_INLINE void stop_profiling() {
        // Clean up profiling resources
    }
} // namespace senkaid::utils::config::debug

// ==========================================
// Debug Breakpoints
// ==========================================

#if defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_DEBUG_BREAK __debugbreak()
#elif defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_DEBUG_BREAK __builtin_trap()
#else
    #define SENKAID_DEBUG_BREAK
#endif

// ==========================================
// Memory Debugging
// ==========================================

#if SENKAID_ENABLE_DEBUG
    #define SENKAID_MEMORY_TRACKING 1
    #define SENKAID_TRACK_ALLOC(ptr, size) \
        senkaid::utils::config::debug::track_allocation(ptr, size, std::source_location::current())
    #define SENKAID_TRACK_FREE(ptr) \
        senkaid::utils::config::debug::track_deallocation(ptr, std::source_location::current())
#else
    #define SENKAID_MEMORY_TRACKING 0
    #define SENKAID_TRACK_ALLOC(ptr, size)
    #define SENKAID_TRACK_FREE(ptr)
#endif

namespace senkaid::utils::config::debug {
    // Memory allocation tracking
    SENKAID_FORCE_INLINE void track_allocation(
        void* ptr,
        std::size_t size,
        std::source_location loc = std::source_location::current()) {
        // Log allocation (e.g., store in global map)
    }

    SENKAID_FORCE_INLINE void track_deallocation(
        void* ptr,
        std::source_location loc = std::source_location::current()) {
        // Log deallocation, check for double-free
    }

    // Check for memory leaks
    SENKAID_FORCE_INLINE void check_memory_leaks() {
        // Report unfreed allocations
    }
} // namespace senkaid::utils::config::debug

// ==========================================
// Compile-Time Validation
// ==========================================

#if SENKAID_LOG_LEVEL > 3
    #error "Invalid SENKAID_LOG_LEVEL; must be 0 (None), 1 (Error), 2 (Warning), or 3 (Info)"
#endif

#if SENKAID_CHECK_NUMERICS && !SENKAID_ENABLE_DEBUG
    #warning "Numerical checks enabled without debug mode; performance may be impacted"
#endif

#if SENKAID_MEMORY_TRACKING && !SENKAID_ENABLE_DEBUG
    #error "Memory tracking requires debug mode"
#endif
