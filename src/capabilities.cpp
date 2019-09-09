#include <omp.h>
#include <immintrin.h>
#include <iostream>

#include "capabilities.h"

#define DEFAULT_BLOCK_WIDTH -1

static int getSimdPixels() {
    char *simdType = std::getenv("MORPH_SIMD");
    if (simdType == nullptr) {
        return
#ifdef MORPH_ENABLE_SIMD_AVX512F
            __builtin_cpu_supports("avx512f") ? 64 :
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                __builtin_cpu_supports("avx2") ? 32 :
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                __builtin_cpu_supports("sse2") ? 16 :
#endif
                DEFAULT_BLOCK_WIDTH;
    } else
#ifdef MORPH_ENABLE_SIMD_AVX512F
    if (strcmp(simdType, "AVX512") == 0) {
        if (__builtin_cpu_supports("avx512f"))
            return 64;
        else
            throw std::runtime_error("This machine does not support AVX512");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
    if (strcmp(simdType, "AVX2") == 0) {
        if (__builtin_cpu_supports("avx2"))
            return 32;
        else
            throw std::runtime_error("This machine does not support AVX2");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
    if (strcmp(simdType, "SSE2") == 0) {
        if (__builtin_cpu_supports("sse2"))
            return 16;
        else
            throw std::runtime_error("This machine does not support SSE2");
    } else
#endif
    if (strcmp(simdType,"none")==0)
        return DEFAULT_BLOCK_WIDTH;
    else
        throw std::runtime_error("Unrecognized SIMD extension type");
}

ParallelConfig makeParallelConfig() {
    char *numCoresStr = std::getenv("MORPH_THREADS");
    int numCores = (numCoresStr == nullptr) ?
                   omp_get_num_procs() : std::atoi(numCoresStr);
    std::cerr << numCores << std::endl;

    ParallelConfig conf{numCores, getSimdPixels()};


    return conf;
}