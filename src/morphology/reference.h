#ifndef MORPH_REFERENCE_H
#define MORPH_REFERENCE_H

#include <opencv2/core/mat.hpp>

class StrEl;

namespace morph {

    cv::Mat dilate(const cv::Mat &image, const StrEl &strEl, int nThreads);

    cv::Mat erode(const cv::Mat &image, const StrEl &strEl, int nThreads);

}

#endif //MORPH_REFERENCE_H
