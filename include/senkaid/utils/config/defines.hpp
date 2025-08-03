#pragma once

#include <cstdint>

// defines.hpp: Defines core types, constants, and utility macros for the senkaid library.
// Provides standardized types, numerical constants, and helper macros for consistent usage.
// Self-contained with no dependencies on other utils/config/*.hpp files; all macros have defaults.

// --- Standard Types ---

// SENKAID_DEFAULT_INT: Default integer type for indexing and integer operations.
// Default: int32_t for compatibility across platforms.
#ifndef SENKAID_DEFAULT_INT
    #define SENKAID_DEFAULT_INT int32_t
#endif

// SENKAID_DEFAULT_SIZE: Default size type for array dimensions and counts.
// Default: size_t for standard C++ compatibility.
#ifndef SENKAID_DEFAULT_SIZE
    #define SENKAID_DEFAULT_SIZE size_t
#endif

// SENKAID_DEFAULT_FLOAT: Default floating-point type for matrix/tensor operations.
// Default: double for high precision.
#ifndef SENKAID_DEFAULT_FLOAT
    #define SENKAID_DEFAULT_FLOAT double
#endif

// --- Numerical Constants ---

// SENKAID_EPSILON: Small value for floating-point comparisons.
// Default: Machine epsilon for SENKAID_DEFAULT_FLOAT.
#ifndef SENKAID_EPSILON
    #if defined(SENKAID_DEFAULT_FLOAT) && SENKAID_DEFAULT_FLOAT == float
        #define SENKAID_EPSILON 1.192092896e-7f
    #else
        #define SENKAID_EPSILON 2.2204460492503131e-16
    #endif
#endif

// SENKAID_PI: Mathematical constant pi.
#ifndef SENKAID_PI
    #define SENKAID_PI 3.14159265358979323846
#endif

// SENKAID_INF: Positive infinity for floating-point types.
#ifndef SENKAID_INF
    #include <cmath>
    #define SENKAID_INF std::numeric_limits<SENKAID_DEFAULT_FLOAT>::infinity()
#endif

// SENKAID_NAN: Not-a-number for floating-point types.
#ifndef SENKAID_NAN
    #include <cmath>
    #define SENKAID_NAN std::numeric_limits<SENKAID_DEFAULT_FLOAT>::quiet_NaN()
#endif

// --- Utility Macros ---

// SENKAID_STRINGIFY: Converts a value to a string literal.
#ifndef SENKAID_STRINGIFY
    #define SENKAID_STRINGIFY(x) #x
#endif

// SENKAID_CONCAT: Concatenates two tokens.
#ifndef SENKAID_CONCAT
    #define SENKAID_CONCAT(a, b) a##b
#endif

// SENKAID_MIN: Returns the minimum of two values.
#ifndef SENKAID_MIN
    #define SENKAID_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// SENKAID_MAX: Returns the maximum of two values.
#ifndef SENKAID_MAX
    #define SENKAID_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// SENKAID_CLAMP: Clamps a value to the range [min, max].
#ifndef SENKAID_CLAMP
    #define SENKAID_CLAMP(value, min, max) SENKAID_MAX(min, SENKAID_MIN(value, max))
#endif

// SENKAID_ALIGNED: Specifies memory alignment for variables or types.
// Default: 64 bytes for SIMD and cache-line compatibility.
#ifndef SENKAID_ALIGNED
    #if defined(_MSC_VER)
        #define SENKAID_ALIGNED(n) __declspec(align(n))
    #else
        #define SENKAID_ALIGNED(n) __attribute__((aligned(n)))
    #endif
    #ifndef SENKAID_DEFAULT_ALIGNMENT
        #define SENKAID_DEFAULT_ALIGNMENT 16
    #endif
#endif

// --- Type Safety and Constraints ---

// SENKAID_RESTRICT: Restricts pointer aliasing for optimization.
#ifndef SENKAID_RESTRICT
    #if defined(_MSC_VER)
        #define SENKAID_RESTRICT __restrict
    #else
        #define SENKAID_RESTRICT __restrict__
    #endif
#endif

// SENKAID_NOEXCEPT: Specifies noexcept for functions where exceptions are not thrown.
#ifndef SENKAID_NOEXCEPT
    #if __cplusplus >= 201103L
        #define SENKAID_NOEXCEPT noexcept
    #else
        #define SENKAID_NOEXCEPT
    #endif
#endif

// SENKAID_CONSTEXPR: Marks functions or variables as constexpr where supported.
#ifndef SENKAID_CONSTEXPR
    #if __cplusplus >= 201402L
        #define SENKAID_CONSTEXPR constexpr
    #else
        #define SENKAID_CONSTEXPR
    #endif
#endif

// SENKAID_INLINE: Forces inlining for performance-critical functions.
#ifndef SENKAID_INLINE
    #if defined(_MSC_VER)
        #define SENKAID_INLINE __forceinline
    #else
        #define SENKAID_INLINE inline __attribute__((always_inline))
    #endif
#endif

// --- Memory and Ownership ---

// SENKAID_NO_UNIQUE_ADDRESS: Enables [[no_unique_address]] for empty classes/structs.
#ifndef SENKAID_NO_UNIQUE_ADDRESS
    #if __cplusplus >= 202002L
        #define SENKAID_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #else
        #define SENKAID_NO_UNIQUE_ADDRESS
    #endif
#endif

#define SENKAID_DISABLE_COPY(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete;

// --- Miscellaneous ---

// SENKAID_UNREACHABLE: Marks code paths as unreachable for optimization and diagnostics.
#ifndef SENKAID_UNREACHABLE
    #if defined(__clang__) || defined(__GNUC__)
        #define SENKAID_UNREACHABLE __builtin_unreachable()
    #elif defined(_MSC_VER)
        #define SENKAID_UNREACHABLE __assume(0)
    #else
        #define SENKAID_UNREACHABLE
    #endif
#endif

// SENKAID_LIKELY/UNLIKELY: Hints for branch prediction.
#ifndef SENKAID_LIKELY
    #if defined(__clang__) || defined(__GNUC__)
        #define SENKAID_LIKELY(x) __builtin_expect(!!(x), 1)
        #define SENKAID_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else
        #define SENKAID_LIKELY(x) (x)
        #define SENKAID_UNLIKELY(x) (x)
    #endif
#endif
