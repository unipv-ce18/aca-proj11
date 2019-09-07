#ifndef MORPH_KERNELS_H
#define MORPH_KERNELS_H

#include <immintrin.h>
#include <opencv2/core/mat.hpp>

#include "../parallel/Chunk.h"
#include "StrEl.h"

#define KERN_ARGS               cv::Mat &out, const cv::Mat &src, const StrEl &strEl, const parallel::Chunk &ch
#define KERN_METHOD(op, var)    inline void _k_##op##_##var(KERN_ARGS)

namespace morph {
    namespace kern {

        KERN_METHOD(dilate, safe) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, safe) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(dilate, single) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, single) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#ifdef MORPH_ENABLE_SIMD_SSE2

        KERN_METHOD(dilate, sse2) {
#define K_METHOD_DILATE
#define K_SIMD_TYPE_SSE2
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, sse2) {
#define K_METHOD_ERODE
#define K_SIMD_TYPE_SSE2
#include "kern_sched_cpu.inl"
        }

#endif

#ifdef MORPH_ENABLE_SIMD_AVX2

        KERN_METHOD(dilate, avx2) {
#define K_METHOD_DILATE
#define K_SIMD_TYPE_AVX2
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, avx2) {
#define K_METHOD_ERODE
#define K_SIMD_TYPE_AVX2
#include "kern_sched_cpu.inl"
        }

#endif

    }
}

#endif //MORPH_KERNELS_H
