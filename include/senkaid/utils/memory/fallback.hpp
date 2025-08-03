#pragma once

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>
#include "utils.hpp"

#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint>
    #endif
    #if SENKAID_HAS_INCLUDE(<memory>)
        #include <memory>
    #endif
#else
    #include <cstdint>
    #include <memory>
#endif

namespace senkaid::memory {

class FallbackAllocator {
public:
    explicit FallbackAllocator(std::size_t _size, std::size_t _alignment = alignof(std::max_align_t))
        : _buffer(nullptr), _size(_size), _alignment(_alignment), _offset(0) {
        SENKAID_ASSERT_CRITICAL(_size > 0, "FallbackAllocator: invalid size");
        SENKAID_ASSERT_CRITICAL(is_valid_alignment(_alignment), "FallbackAllocator: invalid alignment");
        _buffer = static_cast<std::uint8_t*>(::operator new(_size, std::align_val_t{_alignment}));
        SENKAID_ASSERT_CRITICAL(_buffer != nullptr, "FallbackAllocator: allocation failed");
    }

    ~FallbackAllocator() {
        if (_buffer) {
            ::operator delete(_buffer, std::align_val_t{_alignment});
        }
    }

    FallbackAllocator(const FallbackAllocator&) = delete;
    FallbackAllocator& operator=(const FallbackAllocator&) = delete;

    FallbackAllocator(FallbackAllocator&& other) noexcept
        : _buffer(other._buffer), _size(other._size), _alignment(other._alignment), _offset(other._offset) {
        other._buffer = nullptr;
        other._size = 0;
        other._alignment = 0;
        other._offset = 0;
    }

    FallbackAllocator& operator=(FallbackAllocator&& other) noexcept {
        if (this != &other) {
            if (_buffer) {
                ::operator delete(_buffer, std::align_val_t{_alignment});
            }
            _buffer = other._buffer;
            _size = other._size;
            _alignment = other._alignment;
            _offset = other._offset;
            other._buffer = nullptr;
            other._size = 0;
            other._alignment = 0;
            other._offset = 0;
        }
        return *this;
    }

    template<typename T>
    SENKAID_FORCE_INLINE T* allocate(std::size_t count) {
        std::size_t alloc_size = count * sizeof(T);
        std::size_t align = std::max(alignof(T), _alignment);
        SENKAID_ASSERT(is_valid_alignment(align), "FallbackAllocator: invalid alignment for type");

        void* current = _buffer + _offset;
        std::size_t space = _size - _offset;
        void* aligned = current;
        if (std::align(align, sizeof(T), aligned, space) == nullptr) {
            SENKAID_LOG_ERROR("FallbackAllocator: insufficient space for allocation");
            // SENKAID_ASSERT(false, "FallbackAllocator: allocation failed");
            return nullptr;
        }

        std::size_t new_offset = static_cast<std::uint8_t*>(aligned) - _buffer + alloc_size;
        if (new_offset > _size) {
            SENKAID_LOG_ERROR("FallbackAllocator: allocation exceeds buffer size");
            // SENKAID_ASSERT(false, "FallbackAllocator: allocation overflow");
            return nullptr;
        }

        _offset = new_offset;
        zero_memory(aligned, alloc_size);
        return static_cast<T*>(aligned);
    }

    template<typename T>
    SENKAID_FORCE_INLINE void deallocate(T* ptr, std::size_t count) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            SENKAID_LOG_WARNING("FallbackAllocator: null pointer deallocation");
            return;
        }
        std::size_t alloc_size = count * sizeof(T);
        std::size_t ptr_offset = static_cast<std::uint8_t*>(static_cast<void*>(ptr)) - _buffer;
        if (ptr_offset >= _size || ptr_offset + alloc_size > _size) {
            SENKAID_LOG_WARNING("FallbackAllocator: invalid pointer deallocation");
            SENKAID_ASSERT(false, "FallbackAllocator: pointer not from allocator");
            return;
        }
        // No actual deallocation; just reset if at the end
        if (ptr_offset + alloc_size == _offset) {
            _offset = ptr_offset;
        }
    }

    SENKAID_FORCE_INLINE void reset() {
        _offset = 0;
    }

    SENKAID_FORCE_INLINE std::size_t used() const {
        return _offset;
    }

    SENKAID_FORCE_INLINE std::size_t remaining() const {
        return _size - _offset;
    }

private:
    static constexpr bool is_valid_alignment(std::size_t alignment) {
        return alignment > 0 && (alignment & (alignment - 1)) == 0;
    }

    std::uint8_t* _buffer;
    std::size_t _size;
    std::size_t _alignment;
    std::size_t _offset;
};

} // namespace senkaid::memory
