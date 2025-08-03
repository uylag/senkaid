#pragma once

// export.hpp: Defines macros for controlling symbol visibility in the senkaid library.
// Ensures proper export/import of symbols for shared libraries and minimizes symbol bloat.
// All macros are prefixed with SENKAID_ to avoid conflicts and can be overridden via CMake.

// SENKAID_EXPORT: Marks functions, classes, or variables for export in shared libraries.
// Used when building the senkaid library as a shared library (DLL on Windows).
// SENKAID_IMPORT: Marks symbols for import when linking against the senkaid library.
// Platform-specific implementations for Windows (DLL) and Unix-like systems (visibility).

#ifdef _WIN32
    // Windows-specific export/import for DLLs
    #ifdef SENKAID_BUILD_DLL
        // Building the senkaid library as a shared library
        #define SENKAID_EXPORT __declspec(dllexport)
    #else
        // Linking against the senkaid shared library
        #define SENKAID_EXPORT __declspec(dllimport)
    #endif
    #define SENKAID_IMPORT __declspec(dllimport)
#else
    // Unix-like systems (Linux, macOS) use GCC/Clang visibility attributes
    #define SENKAID_EXPORT __attribute__((visibility("default")))
    #define SENKAID_IMPORT
#endif

// SENKAID_HIDDEN: Explicitly hides symbols to reduce binary size and prevent unintended exports.
// Supported on Unix-like systems with GCC/Clang; no-op on Windows.
#ifndef _WIN32
    #define SENKAID_HIDDEN __attribute__((visibility("hidden")))
#else
    #define SENKAID_HIDDEN
#endif

// SENKAID_API: Convenience macro for public API symbols.
// Applies SENKAID_EXPORT when building the library, otherwise SENKAID_IMPORT.
#ifdef SENKAID_BUILD_DLL
    #define SENKAID_API SENKAID_EXPORT
#else
    #define SENKAID_API SENKAID_IMPORT
#endif

// SENKAID_NO_EXPORT: Marks symbols that should not be exported, even in shared libraries.
// Useful for internal functions or classes.
#define SENKAID_NO_EXPORT SENKAID_HIDDEN

// SENKAID_EXPORT_TEMPLATE: Explicitly exports template instantiations.
// Used for common types (e.g., Matrix<float>, Matrix<double>) to reduce template bloat.
#ifdef _WIN32
    #define SENKAID_EXPORT_TEMPLATE __declspec(dllexport)
#else
    #define SENKAID_EXPORT_TEMPLATE __attribute__((visibility("default")))
#endif

// SENKAID_EXPORT_CLASS: Convenience macro for exporting entire classes.
// Ensures all class members are exported correctly.
#define SENKAID_EXPORT_CLASS SENKAID_EXPORT

// SENKAID_EXPORT_FUNCTION: Convenience macro for exporting standalone functions.
#define SENKAID_EXPORT_FUNCTION SENKAID_EXPORT

#ifdef SENKAID_BUILD_DLL
    #define SENKAID_EXTERN_TEMPLATE extern template class SENKAID_EXPORT
#else
    #define SENKAID_EXTERN_TEMPLATE extern template class
#endif

