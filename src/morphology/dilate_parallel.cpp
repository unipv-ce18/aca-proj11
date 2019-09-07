#include "dilate_parallel.h"

#include "kernels.h"
#include "../parallel/Plan.h"
#include "StrEl.h"

#include <omp.h>

cv::Mat dilateParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl) {
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
                    if (ch.rect.w == 16)
                        _k_dilate_sse2(out, image, strEl, ch);
                    else
                        _k_dilate_single(out, image, strEl, ch);
                    break;
                default:
                    _k_dilate_safe(out, image, strEl, ch);
                    break;
            }
        }
    }

    return out;
}
