#ifndef MORPH_KERNELS_H
#define MORPH_KERNELS_H

#include "morphology/operator_types.h"
#include "morphology/StrEl.h"
#include "parallel/Chunk.h"

#include <immintrin.h>
#include <opencv2/core/mat.hpp>

#include <cassert>

#define KERN_METHOD_DEF(var) \
    template<typename Operator> \
    inline void _kernel_##var(typename KernelParams<Operator>::type &args, const parallel::Chunk &ch)

#define KERN_METHOD(op, var) \
    template<> inline void _kernel_##var<op>(typename KernelParams<op>::type &args, const parallel::Chunk &ch)

namespace morph {
    namespace kern {

// Ensure SIMD is disabled
#undef SIMD_WIDTH

        KERN_METHOD_DEF(single);        // Non-SIMD (single-pixel) kernels
        KERN_METHOD_DEF(single_safe);   // "safe" kernels with border checks
        KERN_METHOD_DEF(single_safev);  // "semi-safe" kernels with vertical border checks

        KERN_METHOD(Dilate, single) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Dilate, single_safe) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Dilate, single_safev) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, single) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, single_safe) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, single_safev) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, single) {
#define K_METHOD_SKELPART
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, single_safe) {
#define K_METHOD_SKELPART
#define K_ENABLE_BORDER_CHECKS
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, single_safev) {
#define K_METHOD_SKELPART
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }



// ----- SSE2 kernels -----

#ifdef MORPH_ENABLE_SIMD_SSE2
#include "simddefs.sse2.inl"

        KERN_METHOD_DEF(sse2);
        KERN_METHOD_DEF(sse2_safev);

        KERN_METHOD(Dilate, sse2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Dilate, sse2_safev) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, sse2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, sse2_safev) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, sse2) {
#define K_METHOD_SKELPART
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, sse2_safev) {
#define K_METHOD_SKELPART
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif



// ----- AVX2 kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX2
#include "simddefs.avx2.inl"

        KERN_METHOD_DEF(avx2);
        KERN_METHOD_DEF(avx2_safev);

        KERN_METHOD(Dilate, avx2) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Dilate, avx2_safev) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx2) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx2_safev) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, avx2) {
#define K_METHOD_SKELPART
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, avx2_safev) {
#define K_METHOD_SKELPART
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif



// ----- AVX512F kernels -----

#ifdef MORPH_ENABLE_SIMD_AVX512F
#include "simddefs.avx512f.inl"

        KERN_METHOD_DEF(avx512f);
        KERN_METHOD_DEF(avx512f_safev);

        KERN_METHOD(Dilate, avx512f) {
#define K_METHOD_DILATE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Dilate, avx512f_safev) {
#define K_METHOD_DILATE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx512f) {
#define K_METHOD_ERODE
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(Erode, avx512f_safev) {
#define K_METHOD_ERODE
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, avx512f) {
#define K_METHOD_SKELPART
#include "kern_sched_cpu.inl"
        }

        KERN_METHOD(SkelIter, avx512f_safev) {
#define K_METHOD_SKELPART
#define K_ENABLE_BORDER_CHECKS_VERTICAL
#include "kern_sched_cpu.inl"
        }

#include "simddefs_clear.inl"
#endif

    } // namespace kern
} // namespace morph

#endif //MORPH_KERNELS_H
