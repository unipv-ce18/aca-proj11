#ifndef MORPH_KERNELS_H
#define MORPH_KERNELS_H

#include <immintrin.h>
#include <opencv2/core/mat.hpp>

#include "operator_types.h"
#include "../parallel/Chunk.h"
#include "StrEl.h"

#define KERN_PARAMS cv::Mat &out, const cv::Mat &src, const StrEl &strEl, const parallel::Chunk &ch

#define KERN_METHOD_DEF(var) \
    template<typename Operator> inline void _k_##var(KERN_PARAMS)

#define KERN_METHOD(op, var) \
    template<> inline void _k_##var<op>(KERN_PARAMS)

namespace morph {
    namespace kern {

// Ensure SIMD is disabled
#undef SIMD_WIDTH

        KERN_METHOD_DEF(safe);

        KERN_METHOD(Dilate, safe) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, safe) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD_DEF(single);

        KERN_METHOD(Dilate, single) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, single) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }


// ----- SSE2 kernels -----

#ifdef MORPH_ENABLE_SIMD_SSE2
#include "simddefs.sse2.inl"

        KERN_METHOD_DEF(sse2);

        KERN_METHOD(Dilate, sse2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, sse2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif


// ----- AVX2 kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX2
#include "simddefs.avx2.inl"

        KERN_METHOD_DEF(avx2);

        KERN_METHOD(Dilate, avx2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif


// ----- AVX512F kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX512F
#include "simddefs.avx512f.inl"

        KERN_METHOD_DEF(avx512f);

        KERN_METHOD(Dilate, avx512f) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx512f) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif

    } // namespace kern
} // namespace morph

#endif //MORPH_KERNELS_H
