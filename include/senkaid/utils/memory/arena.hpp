#pragma once

// arena.hpp: Memory arena allocator for the senkaid library.
// Provides fast allocation from a pre-allocated memory pool.
// Uses macros from compiler.hpp for compiler detection and support.

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>

// Conditional includes for portability
#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint> // For std::size_t
    #endif
    #if SENKAID_HAS_INCLUDE(<memory>)
        #include <memory> // For std::align, std::align_val_t
    #endif
    #if SENKAID_HAS_INCLUDE(<new>)
        #include <new> // For ::operator new/delete
    #endif
#else
    #include <cstdint>
    #include <memory>
    #include <new>
#endif

namespace senkaid::memory {

// Arena: A simple bump-pointer allocator.
// Allocates raw memory from a large, pre-allocated block.
// Deallocation is not supported individually; the entire arena is reset or destroyed.
class Arena {
public:
    // Constructor: Allocates a large block of memory.
    // Parameters:
    //   size      - The total size of the arena in bytes.
    //   alignment - The minimum alignment for allocated blocks (must be a power of two).
    //               Defaults to the maximum guaranteed alignment.
    explicit Arena(std::size_t size, std::size_t alignment = alignof(std::max_align_t))
        : _size(size), _alignment(alignment), _ptr(nullptr), _offset(0) {
        // Use SENKAID_ASSERT_CRITICAL for conditions that must hold for the Arena to be valid.
        SENKAID_ASSERT_CRITICAL(is_valid_alignment(alignment), "Invalid alignment for Arena");
        // Allocate raw memory with specified alignment.
        // Note: ::operator new can throw std::bad_alloc. A nothrow version is available
        // if exceptions are disabled or different error handling is required.
        _ptr = ::operator new(size, std::align_val_t{alignment});
        // If ::operator new succeeded, _ptr is guaranteed not to be nullptr.
        // If it failed, it would have thrown std::bad_alloc, so we don't need to check _ptr here.
        // The assert below is redundant given standard new behavior, but kept for explicit intent.
        SENKAID_ASSERT_CRITICAL(_ptr != nullptr, "Arena allocation failed (unexpected if new throws)");
    }

    // Destructor: Deallocates the entire memory block.
    ~Arena() {
        if (_ptr) {
            ::operator delete(_ptr, std::align_val_t{_alignment});
        }
    }

    // Disable copy constructor and copy assignment operator.
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    // Move constructor: Transfers ownership of the memory block.
    Arena(Arena&& other) noexcept
        : _size(other._size), _alignment(other._alignment), _ptr(other._ptr), _offset(other._offset) {
        other._ptr = nullptr;
        other._offset = 0;
    }

    // Move assignment operator: Transfers ownership of the memory block.
    Arena& operator=(Arena&& other) noexcept {
        if (this != &other) {
            if (_ptr) {
                ::operator delete(_ptr, std::align_val_t{_alignment});
            }
            _size = other._size;
            _alignment = other._alignment;
            _ptr = other._ptr;
            _offset = other._offset;
            other._ptr = nullptr;
            other._offset = 0;
        }
        return *this;
    }

    // allocate: Allocates a block of memory for 'count' objects of type T.
    // Parameters:
    //   count - The number of objects of type T to allocate space for.
    // Returns:
    //   A pointer to the allocated, properly aligned memory block, or nullptr on failure.
    template<typename T>
    SENKAID_FORCE_INLINE T* allocate(std::size_t count) {
        std::size_t requested_size = count * sizeof(T);
        // Determine the required alignment: the larger of the type's alignment and the arena's minimum.
        std::size_t required_align = std::max(alignof(T), _alignment);
        SENKAID_ASSERT_CRITICAL(is_valid_alignment(required_align), "Invalid alignment for type T or Arena");

        // Calculate the current position and available space.
        void* current_ptr = static_cast<std::uint8_t*>(_ptr) + _offset;
        std::size_t available_space = _size - _offset;

        // Attempt to find an aligned block within the available space.
        void* aligned_ptr = current_ptr;
        // std::align modifies aligned_ptr and available_space if successful.
        if (std::align(required_align, requested_size, aligned_ptr, available_space) == nullptr) {
            // Not enough space or cannot align within the remaining space.
            SENKAID_DEBUG_ONLY(SENKAID_LOG_ERROR("Arena: insufficient space for allocation");)
            // Use SENKAID_ASSERT for conditions that might be disabled in release builds
            // or handled by returning nullptr. Alternatively, use SENKAID_ASSERT_CRITICAL if
            // out-of-memory here is a fatal logic error.
            // Using SENKAID_ASSERT instead of SENKAID_ASSERT_ENABLED avoids -Wtemplate-body issues.
            // SENKAID_ASSERT(false, "Arena: allocation failed due to insufficient space");
            return nullptr;
        }

        // Calculate the new offset after this allocation.
        std::size_t new_offset = static_cast<std::uint8_t*>(aligned_ptr) - static_cast<std::uint8_t*>(_ptr) + requested_size;
        // This check is a safeguard, though std::align should prevent it.
        if (SENKAID_UNLIKELY(new_offset > _size)) {
             SENKAID_DEBUG_ONLY(SENKAID_LOG_ERROR("Arena: allocation logic error (overflow)");)
             // Using SENKAID_ASSERT instead of SENKAID_ASSERT_ENABLED avoids -Wtemplate-body issues.
             // SENKAID_ASSERT(false, "Arena: allocation overflow (internal logic error)");
             return nullptr;
        }

        // Commit the allocation by updating the offset.
        _offset = new_offset;
        return static_cast<T*>(aligned_ptr);
    }

    // reset: Resets the arena, making all previously allocated memory available again.
    // Note: Destructors for objects in the arena are NOT called.
    SENKAID_FORCE_INLINE void reset() {
        _offset = 0;
    }

    // used: Returns the total number of bytes currently allocated from the arena.
    SENKAID_FORCE_INLINE std::size_t used() const {
        return _offset;
    }

    // remaining: Returns the number of bytes still available for allocation.
    SENKAID_FORCE_INLINE std::size_t remaining() const {
        return _size - _offset;
    }

private:
    // is_valid_alignment: Checks if an alignment value is a power of two.
    // This is a helper for assertions.
    static SENKAID_FORCE_INLINE bool is_valid_alignment(std::size_t alignment) {
        return alignment > 0 && (alignment & (alignment - 1)) == 0;
    }

    // --- Member variables ---
    std::size_t _size;      // Total size of the arena.
    std::size_t _alignment; // Minimum guaranteed alignment.
    void* _ptr;             // Pointer to the start of the allocated memory block.
    std::size_t _offset;    // Current offset (bump pointer) within the block.
};

} // namespace senkaid::memory
