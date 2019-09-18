#ifndef MORPH_STREL_H
#define MORPH_STREL_H

#include <opencv2/core/mat.hpp>

#include <vector>
#include <ostream>

class StrEl {

    int cX_;
    int cY_;
    int simdWidth_;
    cv::Size size_;
    std::vector<uint8_t> elem_;
    std::vector<uint8_t> mask_;

public:
    StrEl(const cv::Mat &image, int simdWidth);

    friend std::ostream &operator<<(std::ostream &os, StrEl &el);

    const cv::Size &size() const {
        return size_;
    }

    bool isSet(int x, int y) const {
        return isSetFuzzy(x, y) != 0;
    }

    uint8_t isSetFuzzy(int x, int y) const {
        return mask_[(cY_ + y) * size_.width + cX_ + x];
    }

    uint8_t at(int x, int y) const {
        return elem_[simdWidth_ * ((cY_ + y) * size_.width + cX_ + x)];
    }

    uint8_t &at(int x, int y) {
        return elem_[simdWidth_ * ((cY_ + y) * size_.width + cX_ + x)];
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

    uint8_t *elemPtr() {
        return elem_.data();
    }

    uint8_t *maskPtr() {
        return mask_.data();
    }

};

#endif //MORPH_STREL_H
