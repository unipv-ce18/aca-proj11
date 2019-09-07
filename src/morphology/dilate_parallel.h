#ifndef MORPH_DILATE_PARALLEL_H
#define MORPH_DILATE_PARALLEL_H

#include <opencv2/core/mat.hpp>

// Forward declarations
namespace parallel { class Plan; }
class StrEl;

cv::Mat dilateParallel(parallel::Plan &plan, const cv::Mat &image, const StrEl &strEl);

#endif //MORPH_DILATE_PARALLEL_H
