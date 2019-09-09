#ifndef MORPH_PROCESS_PARALLEL_H
#define MORPH_PROCESS_PARALLEL_H

#include <opencv2/core/mat.hpp>

// Forward declarations
namespace parallel { class Plan; }
class StrEl;

template<typename Operator>
cv::Mat processParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl, bool noSimd);

#endif //MORPH_PROCESS_PARALLEL_H
