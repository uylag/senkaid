#include <iostream>
#include <immintrin.h>
#include <cstring> // For std::memcpy
#include <cmath>
#include <chrono>
#include "senkaid/core/allocator/alignment.hpp"

    void sum_8f (const float* a, const float* b, float* c) {
        __m256 a_vec = _mm256_loadu_ps(a);
        __m256 b_vec = _mm256_loadu_ps(b);
        __m256 c_vec = _mm256_add_ps(a_vec, b_vec);
    
        _mm256_storeu_ps(c, c_vec);
        return;
    }

    bool is_aligned(const void* ptr, size_t alignment) {
        return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
    }

    void load_vs_loadu_benchmark() {
        const size_t size = sizeof(float) * 8;
        size_t alignment = 16; // 16 bites alignment for SSE
        
        float* a = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));
        float b[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

        for (int i = 0; i < 8; ++i) {
            a[i] = static_cast<float>(i + 1);
        }

        const int iterations = 1'000'000;

        // Benchmarking _mm_load_ps
        auto start_load = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            __m128 a_vec = _mm_load_ps(a);
            alignas(16) float sink[4];
            _mm_store_ps(sink, a_vec); // Store to avoid optimization removal
        }

        auto end_load = std::chrono::high_resolution_clock::now();
        auto duration_load = std::chrono::duration_cast<std::chrono::microseconds>(end_load - start_load).count();
        
        // Benchmarking _mm_loadu_ps
        auto start_loadu = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            __m128 b_vec = _mm_loadu_ps(b);
            float sink[4];
            _mm_storeu_ps(sink, b_vec); // Store to avoid optimization removal
        }

        auto end_loadu = std::chrono::high_resolution_clock::now();
        auto duration_loadu = std::chrono::duration_cast<std::chrono::microseconds>(end_loadu - start_loadu).count();

        std::cout << "\n\033[35mIs 'a' aligned? \033[0m" << (is_aligned(a, alignment) ? "Yes" : "No") << '\n';

        senkaid::core::allocator::aligned_free(a);
        a = nullptr; // Reset pointer to avoid dangling pointer

        std::cout << "\033[35mIs 'a' aligned now? \033[0m" << (is_aligned(a, alignment) ? "Yes" : "No") << '\n';

        std::cout << "\033[33m\nBenchmark results:\n";
        float fduration_load = static_cast<float>(duration_load) / 1000;
        float fduration_loadu = static_cast<float>(duration_loadu) / 1000;
        
        std::cout << "\033[0mTime taken by _mm_load_ps: " << fduration_load << " ms\n";
        std::cout << "Time taken by _mm_loadu_ps: " << fduration_loadu << " ms\n";
        std::cout << "Speedup: " << (fduration_loadu / fduration_load) << "x\n";
        std::cout << '\n';
    }

    void elementwise_math(float* a, float* b)
    {
        size_t alignment = 32; // 32 bytes alignment for AVX
        size_t size = sizeof(float) * 8;        float* aligned_a = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));
        std::memcpy(aligned_a, a, size);

        float* aligned_b = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));
        std::memcpy(aligned_b, b, size);

        __m256 a_vec = _mm256_load_ps(aligned_a);
        __m256 b_vec = _mm256_load_ps(aligned_b);

        float* result = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));
        __m256 add_vec = _mm256_add_ps(a_vec, b_vec);
        __m256 sub_vec = _mm256_sub_ps(a_vec, b_vec);
        __m256 mul_vec = _mm256_mul_ps(a_vec, b_vec);
        __m256 rcp_vec = _mm256_rcp_ps(b_vec); // Reciprocal for division
        __m256 div_vec = _mm256_mul_ps(a_vec, rcp_vec); // Division using multiplication by reciprocal

        // === Store results ===
        
        std::cout << "\033[34mStore results in aligned memory: \033[0m\n";

        // --- Store addition result ---
        _mm256_store_ps(result, add_vec);
        std::cout << "Addition result: ";

        for (int i = 0; i < 8; ++i) {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';
        
        // --- Store subtraction result ---
        _mm256_store_ps(result, sub_vec);

        std::cout << "Subtraction result: ";
    
        for (int i = 0; i < 8; ++i) {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';

        // --- Store multiplication result ---
        _mm256_store_ps(result, mul_vec);
        std::cout << "Multiplication result: ";

        for (int i = 0; i < 8; ++i) {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';
        
        // --- Store division result ---
        _mm256_store_ps(result, div_vec);
        std::cout << "Division result: ";
        
        for (int i = 0; i < 8; ++i) {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';

        // Free allocated memory
        senkaid::core::allocator::aligned_free(aligned_a);
        senkaid::core::allocator::aligned_free(aligned_b);
        senkaid::core::allocator::aligned_free(result);
        
        std::cout << '\n';
    }   

    __m256 abs_simd(const __m256& vec) {
        const __m256 abs_mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
        return _mm256_and_ps(vec, abs_mask);
    }

    __m256 negate_simd(const __m256& vec) {
        const __m256 mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000));
        return _mm256_xor_ps(vec, mask);
    }

    // Be aware: mask should be either 0xFFFFFFFF (all bits set) or 0x00000000 (all bits unset)
    __m256 select_simd(const __m256& a, const __m256& b, const __m256& mask) {
        __m256 a_chosen = _mm256_and_ps(a, mask);
        __m256 b_chosen = _mm256_andnot_ps(mask, b);
        return _mm256_or_ps(a_chosen, b_chosen);
    }

    __m256 fused_op_mask(const __m256& a, const __m256& b, const __m256& mask) {
        __m256 less_zero_mask = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(a), 31));
        __m256 neg_b = _mm256_xor_ps(b, _mm256_castsi256_ps(_mm256_set1_epi32(1u << 31)));
        __m256 mul_result = _mm256_mul_ps(a, select_simd(neg_b, b, less_zero_mask));
        return select_simd(a, mul_result, mask);
    }

    float* relu8(const float* a, const size_t& size_of_a, float* out) {
        size_t alignment = 32; // 32 bytes alignment for AVX
        size_t size = size_of_a * sizeof(float); // 8 floats

        float* aligned_a = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));
        float* aligned_out = reinterpret_cast<float*>(senkaid::core::allocator::aligned_malloc(size, alignment));

        int32_t extra = size_of_a % 8;

        std::memcpy(aligned_a, a, size);     

        int i = 0;
        for (; i < size_of_a - extra; i += 8) {
            __m256 a_vec = _mm256_load_ps(aligned_a + i);
            __m256 mask = _mm256_cmp_ps(a_vec, _mm256_setzero_ps(), _CMP_GT_OQ);
            __m256 relu = _mm256_blendv_ps(_mm256_setzero_ps(), a_vec, mask);
            _mm256_store_ps(aligned_out + i, relu);
        }

        for (; i < size_of_a; ++i) {
            aligned_out[i] = (aligned_a[i] > 0.0f) ? aligned_a[i] : 0.0f;
        }

        std::memcpy(out, aligned_out, size);

        senkaid::core::allocator::aligned_free(aligned_a);
        senkaid::core::allocator::aligned_free(aligned_out);

        aligned_a = nullptr; // Reset pointer to avoid dangling pointer
        aligned_out = nullptr; // Reset pointer to avoid dangling pointer

        return out;
    }

int main()
{
    float a[1][1] = {{1}};
    
    std::cout << a[0][0];

    return 0;
}
