#pragma once

#include <senkaid/utils/config/root.hpp>
#include "flags.hpp"

#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint>
    #endif
    #if SENKAID_HAS_INCLUDE(<string>)
        #include <string>
    #endif
    #if SENKAID_HAS_INCLUDE(<sstream>)
        #include <sstream>
    #endif
#else
    #include <cstdint>
    #include <string>
    #include <sstream>
#endif

namespace senkaid::debug {

SENKAID_DIAGNOSTIC_PUSH
SENKAID_IGNORE_WARNING_UNUSED_FUNCTION

SENKAID_DEBUG_ONLY(
    template<typename T>
    SENKAID_FORCE_INLINE std::string dump_memory(const T* ptr, std::size_t size, std::size_t max_elements = 16) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            return "null";
        }
        std::stringstream ss;
        ss << "[";
        const std::size_t display_size = (size > max_elements) ? max_elements : size;
        for (std::size_t i = 0; i < display_size; ++i) {
            ss << ptr[i];
            if (i < display_size - 1) {
                ss << ", ";
            }
        }
        if (size > max_elements) {
            ss << ", ...";
        }
        ss << "]";
        return ss.str();
    }
)

#if defined(SENKAID_COMPILER_NVCC)
    SENKAID_CUDA_HOST_DEVICE SENKAID_DEBUG_ONLY(
        inline void dump_cuda_memory(const float* ptr, std::size_t size, char* buffer, std::size_t buffer_size) {
            if (!ptr) {
                snprintf(buffer, buffer_size, "null");
                return;
            }
            snprintf(buffer, buffer_size, "[%f", ptr[0]);
            const std::size_t max_elements = buffer_size > 64 ? 8 : 4;
            const std::size_t display_size = size > max_elements ? max_elements : size;
            for (std::size_t i = 1; i < display_size; ++i) {
                char temp[32];
                snprintf(temp, sizeof(temp), ", %f", ptr[i]);
                strncat(buffer, temp, buffer_size - strlen(buffer) - 1);
            }
            if (size > max_elements) {
                strncat(buffer, ", ...", buffer_size - strlen(buffer) - 1);
            }
            strncat(buffer, "]", buffer_size - strlen(buffer) - 1);
        }
    )
#else
    SENKAID_DEBUG_ONLY(
        inline void dump_cuda_memory(const float*, std::size_t, char*, std::size_t) {}
    )
#endif

SENKAID_DIAGNOSTIC_POP

} // namespace senkaid::debug
