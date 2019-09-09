#ifndef MORPH_PROCESS_PARALLEL_TCC
#define MORPH_PROCESS_PARALLEL_TCC

#include "process_parallel.h"

#include "kernels.h"
#include "../parallel/Plan.h"
#include "StrEl.h"
#include "../simd_props.h"

#include <omp.h>

template<typename Operator>
cv::Mat processParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl, const bool noSimd) {
    const auto &alloc = plan.effectiveRegions().allocation();

    assert(image.type() == CV_8UC1);
    cv::Mat out(image.size(), CV_8UC1);

    using namespace morph::kern;

#pragma omp parallel default(none) shared(alloc, image, strEl, out) num_threads(alloc.size())
    {
        int core = omp_get_thread_num();

        for (const auto &ch : alloc[core]) {
            switch (ch.type) {
                case CHUNK_REGULAR:
                    if (noSimd) {
                        _k_single<Operator>(out, image, strEl, ch);
                        continue;
                    }

                    switch (ch.rect.w) {
#ifdef MORPH_ENABLE_SIMD_AVX512F
                        case SIMD_WIDTH_AVX512F:
                            _k_avx512f<Operator>(out, image, strEl, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                        case SIMD_WIDTH_AVX2:
                            _k_avx2<Operator>(out, image, strEl,ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                        case SIMD_WIDTH_SSE2:
                            _k_sse2<Operator>(out, image, strEl, ch);
                            break;
#endif
                        default:
                            _k_single<Operator>(out, image, strEl, ch);
                    }

                    break;
                default:
                    _k_safe<Operator>(out, image, strEl, ch);
                    break;
            }
        }
    }

    return out;
}

#endif //MORPH_PROCESS_PARALLEL_TCC
