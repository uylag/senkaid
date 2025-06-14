#pragma once
#include "root.hpp"

#if defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_CLANG)
    #if __has_cpp_attribute(gnu::optimize)
        #define SENKAID_ATTR_FAST [[gnu::optimize("O3", "unroll-loops", "vectorize")]]
    #else
        #define SENKAID_ATTR_FAST
    #endif
#else
    #define SENKAID_ATTR_FAST
#endif

#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_LOOP_UNROLL(N)              SENKAID_PRAGMA(clang loop unroll_count(N))
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_LOOP_UNROLL(N)              SENKAID_PRAGMA(GCC unroll N)
#elif defined(SENKAID_COMPILER_MSVC)
    #warning "Loop unrolling not supported on MSVC; SENKAID_LOOP_UNROLL no-op"
#else
    #define SENKAID_LOOP_UNROLL(N)
#endif

#if defined(SENKAID_COMPILER_CLANG) || defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_AGGR_VECTOR                 SENKAID_PRAGMA(ivdep)
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_AGGR_VECTOR                 SENKAID_PRAGMA(loop(ivdep))
#else
    #define SENKAID_AGGR_VECTOR
#endif

#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_VECTORIZE                   SENKAID_PRAGMA(clang loop vectorize(enable))
#elif defined(SENKAID_COMPILER_GCC) || defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_VECTORIZE                   SENKAID_PRAGMA(vector always)
#else
    #define SENKAID_VECTORIZE
#endif

#if defined(SENKAID_COMPILER_CLANG)
    #define SENKAID_OPTIMIZE(level) SENKAID_PRAGMA(clang optimize level)
#elif defined(SENKAID_COMPILER_GCC)
    #define SENKAID_OPTIMIZE(level) SENKAID_PRAGMA(GCC optimize(level))
#elif defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_OPTIMIZE(level) SENKAID_PRAGMA(optimize(level, on))
#else
    #define SENKAID_OPTIMIZE(level)
#endif

#if (defined(SENKAID_ARCH_X86_64) || defined(SENKAID_ARCH_AARCH64)) && \
    (defined(SENKAID_COMPILER_GCC) || \
     defined(SENKAID_COMPILER_CLANG) || \
     defined(SENKAID_COMPILER_INTEL))
    #define SENKAID_PREFETCH(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
#else
    #define SENKAID_PREFETCH(addr, rw, locality)
#endif

#if defined(SENKAID_ENABLE_OPENMP)
    #define SENKAID_OMP_SIMD                    SENKAID_PRAGMA(omp simd)
    #define SENKAID_OMP_PARALLEL_FOR            SENKAID_PRAGMA(omp parallel for)
    #define SENKAID_OMP_PARALLEL_SIMD           SENKAID_PRAGMA(omp parallel for simd)
    #define SENKAID_OMP_FOR_SIMD                SENKAID_PRAGMA(omp for simd)
    #define SENKAID_OMP_DECLARE_SIMD            SENKAID_PRAGMA(omp declare simd)
    #define SENKAID_OMP_SIMD_REDUCTION(op, var) SENKAID_PRAGMA(omp simd reduction(op:var))
#else
    #define SENKAID_OMP_SIMD
    #define SENKAID_OMP_PARALLEL_FOR
    #define SENKAID_OMP_PARALLEL_SIMD
    #define SENKAID_OMP_FOR_SIMD
    #define SENKAID_OMP_DECLARE_SIMD
    #define SENKAID_OMP_SIMD_REDUCTION(op, var)
#endif

#if defined(SENKAID_COMPILER_CLANG) || \
    defined(SENKAID_COMPILER_GCC) || \
    defined(SENKAID_COMPILER_MSVC) || \
    defined(SENKAID_COMPILER_INTEL)
    #define SENKAID_PACK_PUSH(N)               SENKAID_PRAGMA(pack(push, N))
    #define SENKAID_PACK_POP                   SENKAID_PRAGMA(pack(pop))
#endif

#if defined(SENKAID_COMPILER_MSVC)
    #define SENKAID_LINK_LIB(lib) SENKAID_PRAGMA(comment(lib, lib))
#else
    #define SENKAID_LINK_LIB(lib)
#endif

#if defined(SENKAID_COMPILER_NVCC) || defined(SENKAID_COMPILER_HIPCC) || defined(SENKAID_COMPILER_SYCL)
    #warning "Some pragmas may be ignored by GPU compilers (NVCC, HIPCC, SYCL)"
    #undef SENKAID_LOOP_UNROLL
    #define SENKAID_LOOP_UNROLL(N)
    #undef SENKAID_AGGR_VECTOR
    #define SENKAID_AGGR_VECTOR
    #undef SENKAID_VECTORIZE
    #define SENKAID_VECTORIZE
    #undef SENKAID_OMP_SIMD
    #define SENKAID_OMP_SIMD
    #undef SENKAID_OMP_PARALLEL_FOR
    #define SENKAID_OMP_PARALLEL_FOR
    #undef SENKAID_OMP_PARALLEL_SIMD
    #define SENKAID_OMP_PARALLEL_SIMD
    #undef SENKAID_OMP_FOR_SIMD
    #define SENKAID_OMP_FOR_SIMD
    #undef SENKAID_OMP_DECLARE_SIMD
    #define SENKAID_OMP_DECLARE_SIMD
    #undef SENKAID_OMP_SIMD_REDUCTION
    #define SENKAID_OMP_SIMD_REDUCTION(op, var)
    #undef SENKAID_OMP_PARALLEL
    #define SENKAID_OMP_PARALLEL
    #undef SENKAID_OMP_TASK
    #define SENKAID_OMP_TASK
    #undef SENKAID_OMP_CRITICAL
    #define SENKAID_OMP_CRITICAL
    #undef SENKAID_OMP_BARRIER
    #define SENKAID_OMP_BARRIER
#endif
