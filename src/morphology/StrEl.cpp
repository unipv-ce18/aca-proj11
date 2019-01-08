#include "StrEl.h"

#include <opencv2/core/core.hpp>

static const cv::Size &checkSize(const cv::Size &size) {
    if (size.width % 2 == 0 || size.height % 2 == 0) {
        std::ostringstream msg;
        msg << "Structural element must be odd sized (was " << size.width << 'x' << size.height << " pixels)";
        throw std::runtime_error(msg.str());
    }
    return size;
}

StrEl::StrEl(const cv::Mat &image) : size_(checkSize(image.size())), elem_(size_, CV_8UC1), mask_(size_, CV_8UC1) {
    cv::extractChannel(image, elem_, 2); // Red channel
    cv::extractChannel(image, mask_, 3); // Alpha channel

    cX_ = (size_.width - 1) / 2;
    cY_ = (size_.height - 1) / 2;
}

std::ostream &operator<<(std::ostream &os, const StrEl &el) {
    os << "Structural Element (center: " << (el.cX_ + 1) << ", " << (el.cY_ + 1)
       << ")\n--- Weights ---\n" << el.elem_ << "\n--- Mask ---\n" << el.mask_;
    return os;
}
