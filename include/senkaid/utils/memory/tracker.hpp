#pragma once

#include <senkaid/utils/config/root.hpp>
#include <senkaid/utils/debug/root.hpp>

#if defined(SENKAID_HAS_INCLUDE)
    #if SENKAID_HAS_INCLUDE(<cstdint>)
        #include <cstdint>
    #endif
    #if SENKAID_HAS_INCLUDE(<unordered_map>)
        #include <unordered_map>
    #endif
    #if SENKAID_HAS_INCLUDE(<mutex>)
        #include <mutex>
    #endif
#else
    #include <cstdint>
    #include <unordered_map>
    #include <mutex>
#endif

namespace senkaid::memory {

class MemoryTracker {
public:
    static MemoryTracker& instance() {
        static MemoryTracker tracker;
        return tracker;
    }

   SENKAID_FORCE_INLINE void track_allocation(void* ptr, std::size_t size, const char* file = nullptr, int line = 0) {
       if (SENKAID_UNLIKELY(ptr == nullptr || size == 0)) {
           SENKAID_LOG_WARNING("track_allocation: invalid allocation (null pointer or zero size)");
           return;
       }
       std::lock_guard<std::mutex> lock(_mutex);
       _allocations[ptr] = AllocationInfo{size, file ? file : "unknown", line};
       _total_allocated += size;
       SENKAID_LOG_INFO("Allocated " + std::to_string(size) + " bytes at " + 
                        std::to_string(reinterpret_cast<std::uintptr_t>(ptr)) + 
                        (file ? " (" + std::string(file) + ":" + std::to_string(line) + ")" : ""));
   }
   
   SENKAID_FORCE_INLINE void track_deallocation(void* ptr, const char* file = nullptr, int line = 0) {
       if (SENKAID_UNLIKELY(ptr == nullptr)) {
           SENKAID_LOG_WARNING("track_deallocation: null pointer");
           return;
       }
       std::lock_guard<std::mutex> lock(_mutex);
       auto it = _allocations.find(ptr);
       if (it == _allocations.end()) {
           SENKAID_LOG_WARNING("track_deallocation: pointer not tracked at " + 
                              std::to_string(reinterpret_cast<std::uintptr_t>(ptr)) + 
                              (file ? " (" + std::string(file) + ":" + std::to_string(line) + ")" : ""));
           SENKAID_ASSERT(false, "track_deallocation: untracked pointer");
           return;
       }
       _total_allocated -= it->second.size;
       SENKAID_LOG_INFO("Deallocated " + std::to_string(it->second.size) + " bytes at " + 
                        std::to_string(reinterpret_cast<std::uintptr_t>(ptr)) + 
                        (file ? " (" + std::string(file) + ":" + std::to_string(line) + ")" : ""));
       _allocations.erase(it);
   }

   std::size_t get_total_allocated() const {
       std::lock_guard<std::mutex> lock(_mutex);
       return _total_allocated;
   }

   std::size_t get_allocation_count() const {
       std::lock_guard<std::mutex> lock(_mutex);
       return _allocations.size();
   }

    void report_leaks() const {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_allocations.empty()) {
            SENKAID_LOG_INFO("No memory leaks detected");
            return;
        }
        SENKAID_LOG_WARNING("Detected " + std::to_string(_allocations.size()) + " memory leaks");
        for (const auto& [ptr, info] : _allocations) {
            SENKAID_LOG_WARNING("Leak: " + std::to_string(info.size) + " bytes at " + 
                                std::to_string(reinterpret_cast<std::uintptr_t>(ptr)) + 
                                " (" + info.file + ":" + std::to_string(info.line) + ")");
        }
    }

private:
    MemoryTracker() = default;

    struct AllocationInfo {
        std::size_t size;
        std::string file;
        int line;
    };

    mutable std::mutex _mutex;
    std::unordered_map<void*, AllocationInfo> _allocations;
    std::size_t _total_allocated = 0;
};

#if SENKAID_DEBUG_ENABLED
    #define SENKAID_TRACK_ALLOC(ptr, size) \
        ::senkaid::memory::MemoryTracker::instance().track_allocation(ptr, size, __FILE__, __LINE__)
    #define SENKAID_TRACK_DEALLOC(ptr) \
        ::senkaid::memory::MemoryTracker::instance().track_deallocation(ptr, __FILE__, __LINE__)
#else
    #define SENKAID_TRACK_ALLOC(ptr, size)
    #define SENKAID_TRACK_DEALLOC(ptr)
#endif

} // namespace senkaid::memory
