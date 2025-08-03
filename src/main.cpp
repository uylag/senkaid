#include <iostream>
#include <senkaid/utils/root.hpp>

using namespace senkaid::memory;

int main() {
    std::cout << "[TEST] FallbackAllocator: Basic allocation...\n";
    FallbackAllocator allocator(1024); // 1 KB

    int* int_array = allocator.allocate<int>(10);
    SENKAID_ASSERT(int_array != nullptr, "Failed to allocate int array");
    for (int i = 0; i < 10; ++i) {
        SENKAID_ASSERT(int_array[i] == 0, "Memory not zero-initialized");
    }

    std::cout << "[TEST] FallbackAllocator: Overflow test...\n";
    void* large_block = allocator.allocate<std::uint64_t>(1024); // deliberately too big
    SENKAID_ASSERT(large_block == nullptr, "Expected null from overflow allocation");

    std::cout << "[TEST] FallbackAllocator: Deallocate external pointer (should warn)...\n";
    int* external = new int[5];
    // allocator.deallocate(external, 5); // not from allocator
    delete[] external;

    std::cout << "[TEST] FallbackAllocator: Sequential allocation & release...\n";
    double* a = allocator.allocate<double>(4);
    double* b = allocator.allocate<double>(4);
    SENKAID_ASSERT(a != nullptr && b != nullptr, "Sequential allocations failed");

    allocator.deallocate(b, 4); // release last
    SENKAID_ASSERT(allocator.remaining() >= sizeof(double) * 4, "Release did not free space");

    allocator.deallocate(a, 4); // release second last
    std::cout << "[TEST] FallbackAllocator: Reset...\n";
    allocator.reset();
    SENKAID_ASSERT(allocator.used() == 0, "Reset did not clear used bytes");

    std::cout << "[TEST PASSED] All FallbackAllocator checks passed.\n";
    return 0;
}

