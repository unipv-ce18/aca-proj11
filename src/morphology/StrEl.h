#ifndef MORPH_STREL_H
#define MORPH_STREL_H

#include <opencv2/core/mat.hpp>
#include <ostream>

class StrEl {

    int cX_;
    int cY_;
    cv::Mat elem_;
    cv::Mat mask_;

public:
    explicit StrEl(const cv::Mat &image);

    friend std::ostream &operator<<(std::ostream &os, const StrEl &el);
};

#endif //MORPH_STREL_H
