#ifndef MORPH_KERNELS_H
#define MORPH_KERNELS_H

#include <immintrin.h>
#include <opencv2/core/mat.hpp>

#include "../parallel/Chunk.h"
#include "StrEl.h"

#define KERN_METHOD(op, var) \
    inline void _k_##op##_##var(cv::Mat &out, const cv::Mat &src, const StrEl &strEl, const parallel::Chunk &ch)

namespace morph {
    namespace kern {

// Ensure SIMD is disabled
#undef SIMD_WIDTH

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


// ----- SSE2 kernels -----

#ifdef MORPH_ENABLE_SIMD_SSE2
#include "simddefs.sse2.inl"

        KERN_METHOD(dilate, sse2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, sse2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif


// ----- AVX2 kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX2
#include "simddefs.avx2.inl"

        KERN_METHOD(dilate, avx2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, avx2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif


// ----- AVX512F kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX512F
#include "simddefs.avx512f.inl"

        KERN_METHOD(dilate, avx512f) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(erode, avx512f) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif

    } // namespace kern
} // namespace morph

#endif //MORPH_KERNELS_H
