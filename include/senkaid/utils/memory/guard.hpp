#pragma once

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>

#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint>
    #endif
#else
    #include <cstdint>
#endif

namespace senkaid::memory {

template <typename TN> class MemoryGuard;

template<typename T>
class MemoryGuard<T[]> {
public:
    explicit MemoryGuard(T* ptr, std::size_t count = 1) 
        : _ptr(ptr), _count(count) {
        SENKAID_ASSERT_CRITICAL(ptr != nullptr, "MemoryGuard: null pointer");
        SENKAID_ASSERT_CRITICAL(count > 0, "MemoryGuard: invalid count");
    }

    ~MemoryGuard() {
        if (_ptr) {
            delete[] _ptr;
        }
    }

    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard& operator=(const MemoryGuard&) = delete;

    MemoryGuard(MemoryGuard&& other) noexcept
        : _ptr(other._ptr), _count(other._count) {
        other._ptr = nullptr;
        other._count = 0;
    }

    MemoryGuard& operator=(MemoryGuard&& other) noexcept {
        if (this != &other) {
            if (_ptr) {
                delete[] _ptr;
            }
            _ptr = other._ptr;
            _count = other._count;
            other._ptr = nullptr;
            other._count = 0;
        }
        return *this;
    }

    explicit operator bool() const noexcept
    {
        return _ptr != nullptr;
    }

    SENKAID_FORCE_INLINE T* get() const {
        return _ptr;
    }

    SENKAID_FORCE_INLINE std::size_t count() const {
        return _count;
    }

    SENKAID_FORCE_INLINE void release() {
        _ptr = nullptr;
        _count = 0;
    }

private:
    T* _ptr;
    std::size_t _count;
};


} // namespace senkaid::memory
