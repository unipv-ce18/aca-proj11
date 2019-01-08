#ifndef MORPH_OPERATORS_H
#define MORPH_OPERATORS_H

#include <opencv2/core/mat.hpp>

class StrEl;

namespace morph {

    cv::Mat dilate(const cv::Mat &image, const StrEl &strEl);

    cv::Mat erode(const cv::Mat &image, const StrEl &strEl);

}

#endif //MORPH_OPERATORS_H
