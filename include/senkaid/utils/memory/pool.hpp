#pragma once

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>

#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint>
    #endif
    #if SENKAID_HAS_INCLUDE(<memory>)
        #include <memory>
    #endif
    #if SENKAID_HAS_INCLUDE(<vector>)
        #include <vector>
    #endif
#else
    #include <cstdint>
    #include <memory>
    #include <vector>
#endif

namespace senkaid::memory {

class MemoryPool {
public:
    explicit MemoryPool(std::size_t block_size, std::size_t num_blocks, std::size_t alignment = alignof(std::max_align_t))
        : _block_size(block_size), _alignment(alignment), _free_blocks(num_blocks), _total_blocks(num_blocks) {
        SENKAID_ASSERT_CRITICAL(block_size > 0, "Invalid block size");
        SENKAID_ASSERT_CRITICAL(num_blocks > 0, "Invalid number of blocks");
        SENKAID_ASSERT_CRITICAL(is_valid_alignment(alignment), "Invalid alignment");
        
        std::size_t total_size = block_size * num_blocks;
        void* raw = ::operator new(total_size, std::align_val_t{alignment});
        SENKAID_ASSERT(raw != nullptr, "MemoryPool allocation failed");
        
        _memory = std::unique_ptr<void, Deleter>(raw, Deleter{alignment});
        for (std::size_t i = 0; i < num_blocks; ++i) {
            _free_blocks[i] = static_cast<std::uint8_t*>(raw) + i * block_size;
        }
    }

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept
        : _block_size(other._block_size),
          _alignment(other._alignment),
          _memory(std::move(other._memory)),
          _free_blocks(std::move(other._free_blocks)) {
        other._block_size = 0;
        other._alignment = 0;
        other._total_blocks = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept {
        if (this != &other) {
            _block_size = other._block_size;
            _alignment = other._alignment;
            _memory = std::move(other._memory);
            _free_blocks = std::move(other._free_blocks);
            other._block_size = 0;
            other._alignment = 0;
            _total_blocks = other._total_blocks;
            other._total_blocks = 0;
            other._memory.reset();
        }
        return *this;
    }

    template<typename T>
    SENKAID_FORCE_INLINE T* allocate(std::size_t count = 1) {
        if (SENKAID_UNLIKELY(count * sizeof(T) > _block_size)) {
            SENKAID_LOG_WARNING("MemoryPool: requested size exceeds block size");
            // SENKAID_ASSERT(false, "MemoryPool: allocation size too large");
            return nullptr;
        }
        if (SENKAID_UNLIKELY(_free_blocks.empty())) {
            SENKAID_LOG_WARNING("MemoryPool: no free blocks available");
            // SENKAID_ASSERT(false, "MemoryPool: out of memory");
            return nullptr;
        }

        void* ptr = _free_blocks.back();
        _free_blocks.pop_back();
        return static_cast<T*>(ptr);
    }

    template<typename T>
    SENKAID_FORCE_INLINE void deallocate(T* ptr, std::size_t count = 1) {
        if (SENKAID_UNLIKELY(ptr == nullptr)) {
            SENKAID_LOG_WARNING("MemoryPool: null pointer deallocation");
            return;
        }
        if (SENKAID_UNLIKELY(count * sizeof(T) > _block_size)) {
            SENKAID_LOG_WARNING("MemoryPool: deallocation size exceeds block size");
            // SENKAID_ASSERT(false, "MemoryPool: invalid deallocation size");
            return;
        }
       
        std::uintptr_t base = reinterpret_cast<std::uintptr_t>(_memory.get());
        std::uintptr_t target = reinterpret_cast<std::uintptr_t>(ptr);
        std::size_t offset = target - base;

        if (SENKAID_UNLIKELY(offset % _block_size != 0 || offset >= _block_size * _free_blocks.capacity())) {
            SENKAID_LOG_WARNING("MemoryPool: invalid pointer deallocation");
            // SENKAID_ASSERT(false, "MemoryPool: pointer not from pool");
            return;
        }
        
        _free_blocks.push_back(ptr);
    }

    SENKAID_FORCE_INLINE std::size_t available_blocks() const {
        return _free_blocks.size();
    }

    SENKAID_FORCE_INLINE std::size_t total_blocks() const {
        return _total_blocks;
    }

    SENKAID_FORCE_INLINE std::size_t block_size() const {
        return _block_size;
    }

private:
    struct Deleter {
        std::size_t alignment;
        void operator()(void* ptr) const {
            ::operator delete(ptr, std::align_val_t{alignment});
        }
    };

    static SENKAID_FORCE_INLINE bool is_valid_alignment(std::size_t alignment) {
        return alignment > 0 && (alignment & (alignment - 1)) == 0;
    }

    std::size_t _block_size;
    std::size_t _alignment;
    std::unique_ptr<void, Deleter> _memory;
    std::vector<void*> _free_blocks;
    std::size_t _total_blocks;
};


} // namespace senkaid::memory
