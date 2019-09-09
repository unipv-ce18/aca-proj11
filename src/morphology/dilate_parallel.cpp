#include "dilate_parallel.h"

#include "kernels.h"
#include "../parallel/Plan.h"
#include "StrEl.h"
#include "../simd_props.h"

#include <omp.h>

cv::Mat dilateParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl, const bool noSimd) {
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
                    if (noSimd){
                        _k_dilate_single(out, image, strEl, ch);
                        continue;
                    }

                    switch (ch.rect.w){
#ifdef MORPH_ENABLE_SIMD_AVX512F
                        case SIMD_WIDTH_AVX512F:
                            _k_dilate_avx512f(out, image, strEl, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                        case SIMD_WIDTH_AVX2:
                            _k_dilate_avx2(out, image, strEl,ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                        case SIMD_WIDTH_SSE2:
                            _k_dilate_sse2(out, image, strEl, ch);
                            break;
#endif
                        default:
                            _k_dilate_single(out, image, strEl, ch);
                    }

                    break;
                default:
                    _k_dilate_safe(out, image, strEl, ch);
                    break;
            }
        }
    }

    return out;
}
