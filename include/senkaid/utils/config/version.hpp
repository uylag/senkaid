#pragma once

#include <string>

// Version numbers for semantic versioning (MAJOR.MINOR.PATCH)
#define SENKAID_VERSION_MAJOR 1
#define SENKAID_VERSION_MINOR 0
#define SENKAID_VERSION_PATCH 0

// Version string for user-facing display and runtime checks
#define SENKAID_VERSION_STRING "1.0.0"

// API version for compatibility checks (incremented for breaking changes)
#define SENKAID_API_VERSION 100

// Compile-time version check macro
#define SENKAID_VERSION_ENCODE(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))

// Current encoded version
#define SENKAID_VERSION \
    SENKAID_VERSION_ENCODE(SENKAID_VERSION_MAJOR, SENKAID_VERSION_MINOR, SENKAID_VERSION_PATCH)

// Runtime version query function
namespace senkaid {
    inline std::string get_version() {
        return SENKAID_VERSION_STRING;
    }

    inline int get_api_version() {
        return SENKAID_API_VERSION;
    }

    // Compile-time check for minimum required version
    template<int Major, int Minor, int Patch>
    constexpr bool check_minimum_version() {
        return SENKAID_VERSION >= SENKAID_VERSION_ENCODE(Major, Minor, Patch);
    }
}
