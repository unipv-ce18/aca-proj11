#include "capabilities.h"
#include "simd_props.h"

#include <omp.h>

#include <cstdlib>
#include <cstring>
#include <stdexcept>

static int getSimdPixels() {
    char *simdType = std::getenv("MORPH_SIMD");
    if (simdType == nullptr) {
        return
#ifdef MORPH_ENABLE_SIMD_AVX512F
                __builtin_cpu_supports("avx512f") ? SIMD_WIDTH_AVX512F :
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                __builtin_cpu_supports("avx2") ? SIMD_WIDTH_AVX2 :
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                __builtin_cpu_supports("sse2") ? SIMD_WIDTH_SSE2 :
#endif
                SIMD_WIDTH_NO_SIMD;
    } else
#ifdef MORPH_ENABLE_SIMD_AVX512F
    if (strcmpi(simdType, "AVX512") == 0) {
        if (__builtin_cpu_supports("avx512f"))
            return SIMD_WIDTH_AVX512F;
        else
            throw std::runtime_error("This machine does not support AVX512");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
    if (strcmpi(simdType, "AVX2") == 0) {
        if (__builtin_cpu_supports("avx2"))
            return SIMD_WIDTH_AVX2;
        else
            throw std::runtime_error("This machine does not support AVX2");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
    if (strcmpi(simdType, "SSE2") == 0) {
        if (__builtin_cpu_supports("sse2"))
            return SIMD_WIDTH_SSE2;
        else
            throw std::runtime_error("This machine does not support SSE2");
    } else
#endif
    if (strcmpi(simdType,"none")==0)
        return SIMD_WIDTH_NO_SIMD;
    else
        throw std::runtime_error("Unrecognized SIMD extension type");
}

ParallelConfig makeParallelConfig() {
    char *numCoresStr = std::getenv("MORPH_THREADS");
    int numCores = (numCoresStr == nullptr) ?
                   omp_get_num_procs() : std::atoi(numCoresStr);

    ParallelConfig conf{numCores, getSimdPixels()};


    return conf;
}
