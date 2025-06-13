#pragma once

#include <cstdint>
#include <string_view>

namespace senkaid::utils::config {
    struct version {
        static constexpr inline std::size_t _major = 0;
        static constexpr inline std::size_t _minor = 0;
        static constexpr inline std::size_t _patch = 1;
        static constexpr inline std::size_t _abi = 1;
        static constexpr inline std::string_view _string = "0.0.1";
        static constexpr inline std::string_view _full = "v0.0.1 (ABI 1)";

        [[nodiscard]]
        static constexpr bool at_least(const std::size_t& major, std::size_t& minor, const std::size_t& patch) {
            return (_major > major) ||
                   (_major == major && _minor > minor) ||
                   (_major == major && _minor == minor && _patch >= patch);
        }
    };
} // namespace senkaid::utils::config
