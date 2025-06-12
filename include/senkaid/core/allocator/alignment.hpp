#pragma once
#include <cstdlib>
#include <cstddef>
#include <new>

namespace senkaid::core::allocator {

inline void* aligned_malloc(std::size_t size, std::size_t alignment) {
    if (alignment < alignof(void*) || (alignment & (alignment - 1)) != 0)
        return nullptr; // Alignment must be power of 2 and >= sizeof(void*)

#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);

#elif (defined(__cpp_aligned_new) && __cpp_aligned_new >= 201606L)
    // C++17 guaranteed aligned allocation
    return ::operator new(size, std::align_val_t(alignment), std::nothrow);

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    // C11 aligned_alloc requires size to be multiple of alignment
    std::size_t padded_size = (size + alignment - 1) / alignment * alignment;
    return aligned_alloc(alignment, padded_size);

#elif defined(_POSIX_VERSION)
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0)
        return nullptr;
    return ptr;

#else
    #error "No supported aligned allocation method available on this platform"
#endif
}

inline void aligned_free(void* ptr) {
    if (!ptr) return;

#if defined(_MSC_VER)
    _aligned_free(ptr);

#elif (defined(__cpp_aligned_new) && __cpp_aligned_new >= 201606L)
    ::operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));

#else
    free(ptr);
#endif
}

} // namespace senkaid

