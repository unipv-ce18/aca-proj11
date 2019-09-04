#include "dilate_parallel.h"

#include "StrEl.h"
#include "../parallel/Plan.h"

#include <omp.h>
#include <opencv2/core/core.hpp>
#include <iostream>

inline void procRegular(cv::Mat &out, const cv::Mat &src, const StrEl &strEl, const parallel::Chunk &ch) {
#include "dilate_parallel_kernel.inc"
}

inline void procUnsafe(cv::Mat &out, const cv::Mat &src, const StrEl &strEl, const parallel::Chunk &ch) {
#define ENABLE_BORDER_CHECKS

#include "dilate_parallel_kernel.inc"

#undef ENABLE_BORDER_CHECKS
}

cv::Mat dilateParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl) {

    auto &alloc = plan.effectiveRegions().allocation();
    double start = omp_get_wtime();

    assert(image.type() == CV_8UC1);
    cv::Mat out(image.size(), CV_8UC1);

#pragma omp parallel num_threads(alloc.size())
    {
        int core = omp_get_thread_num();
        for (auto ch = alloc[core].begin(); ch != alloc[core].end(); ++ch) {
            switch (ch->type) {
                case CHUNK_REGULAR:
                    procRegular(out, image, strEl, *ch);
                    break;

                default:
                    procUnsafe(out, image, strEl, *ch);
                    break;

            }

        }


    }


    std::cout << "Execution time" << (omp_get_wtime() - start) << "s\n";
    return out;

}
