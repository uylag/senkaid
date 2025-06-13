#pragma once

/**
 * @file export.hpp
 * @brief Symbol visibility and export/import macros for the Senkaid library.
 *
 * Defines macros for controlling shared library symbol visibility and exports,
 * ensuring portability across platforms and compilers. Integrates with compiler.hpp,
 * platform.hpp, and defines.hpp for consistent configuration.
 */

#include "compiler.hpp"
#include "platform.hpp"
#include "defines.hpp"

// ==========================================
// Shared Library Export/Import Configuration
// ==========================================

// Define SENKAID_BUILD_SHARED to enable shared library build (set via CMake)
#ifndef SENKAID_BUILD_SHARED
    #define SENKAID_BUILD_SHARED 0
#endif

// Export/import macros for shared libraries
#if defined(SENKAID_PLATFORM_WINDOWS)
    #if SENKAID_BUILD_SHARED
        #ifdef SENKAID_BUILDING_LIBRARY
            #define SENKAID_EXPORT __declspec(dllexport)
        #else
            #define SENKAID_EXPORT __declspec(dllimport)
        #endif
        #define SENKAID_NO_EXPORT
    #else
        #define SENKAID_EXPORT
        #define SENKAID_NO_EXPORT
    #endif
#else // POSIX systems (Linux, macOS)
    #if SENKAID_BUILD_SHARED
        #define SENKAID_EXPORT __attribute__((visibility("default")))
        #define SENKAID_NO_EXPORT __attribute__((visibility("hidden")))
    #else
        #define SENKAID_EXPORT
        #define SENKAID_NO_EXPORT
    #endif
#endif

// Class-specific export macro
#define SENKAID_CLASS_EXPORT SENKAID_EXPORT

// Template export macro (no effect on Windows, visibility control on POSIX)
#if defined(SENKAID_PLATFORM_WINDOWS)
    #define SENKAID_TEMPLATE_EXPORT
#else
    #define SENKAID_TEMPLATE_EXPORT SENKAID_EXPORT
#endif

// ==========================================
// ABI Versioning
// ==========================================

// Mark symbols with specific ABI version
#define SENKAID_ABI_VERSIONED(version) \
    SENKAID_EXPORT __attribute__((symver("senkaid@" #version)))

// Default ABI version macro
#define SENKAID_ABI_DEFAULT SENKAID_ABI_VERSIONED(SENKAID_ABI_VERSION)

// ==========================================
// Deprecation Utilities
// ==========================================

#if defined(SENKAID_CXX17)
    #define SENKAID_DEPRECATED_EXPORT(message) \
        SENKAID_EXPORT [[deprecated(message)]]
#else
    #define SENKAID_DEPRECATED_EXPORT(message) SENKAID_EXPORT
#endif

// ==========================================
// Compile-Time Validation
// ==========================================

#if SENKAID_BUILD_SHARED && defined(SENKAID_PLATFORM_UNKNOWN)
    #warning "Shared library build on unknown platform; export behavior undefined"
#endif

#if defined(SENKAID_BUILDING_LIBRARY) && !SENKAID_BUILD_SHARED
    #error "SENKAID_BUILDING_LIBRARY defined but shared library build not enabled"
#endif
