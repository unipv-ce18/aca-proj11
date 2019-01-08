#ifndef MORPH_STREL_H
#define MORPH_STREL_H

#include <opencv2/core/mat.hpp>
#include <ostream>

class StrEl {

    int cX_;
    int cY_;
    cv::Size size_;
    cv::Mat elem_;
    cv::Mat mask_;

public:
    explicit StrEl(const cv::Mat &image);

    friend std::ostream &operator<<(std::ostream &os, const StrEl &el);

    const cv::Size &size() const {
        return size_;
    }

    bool isSet(int x, int y) const {
        return mask_.at<uint8_t>(cX_ + x, cY_ + y) != 0;
    }

    uint8_t at(int x, int y) const {
        return elem_.at<uint8_t>(cX_ + x, cY_ + y);
    }

    int xMin() const {
        return -cX_;
    }

    int xMax() const {
        return cX_;
    }

    int yMin() const {
        return -cY_;
    }

    int yMax() const {
        return cY_;
    }

};

#endif //MORPH_STREL_H
