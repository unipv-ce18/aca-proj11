#include "capabilities.h"
#include "simd_props.h"

#include <omp.h>

#include <algorithm>
#include <cstdlib>
#include <string>
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
    }

    std::string sSimdType(simdType);
    std::transform(sSimdType.begin(), sSimdType.end(), sSimdType.begin(), ::tolower);
#ifdef MORPH_ENABLE_SIMD_AVX512F
    if (sSimdType == "avx512") {
        if (__builtin_cpu_supports("avx512f") && __builtin_cpu_supports("avx512bw"))
            return SIMD_WIDTH_AVX512F;
        else
            throw std::runtime_error("This machine does not support AVX512");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
    if (sSimdType == "avx2") {
        if (__builtin_cpu_supports("avx2"))
            return SIMD_WIDTH_AVX2;
        else
            throw std::runtime_error("This machine does not support AVX2");
    } else
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
    if (sSimdType == "sse2") {
        if (__builtin_cpu_supports("sse2"))
            return SIMD_WIDTH_SSE2;
        else
            throw std::runtime_error("This machine does not support SSE2");
    } else
#endif
    if (sSimdType == "none")
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
