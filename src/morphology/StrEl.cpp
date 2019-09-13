#include "StrEl.h"

#include <iomanip>
#include <sstream>

static const cv::Size &checkSize(const cv::Size &size) {
    if (size.width % 2 == 0 || size.height % 2 == 0) {
        std::ostringstream msg;
        msg << "Structural element must be odd sized (was " << size.width << 'x' << size.height << " pixels)";
        throw std::runtime_error(msg.str());
    }
    return size;
}

StrEl::StrEl(const cv::Mat &image, const int simdWidth) : size_(checkSize(image.size())),
                                                          simdWidth_(simdWidth),
                                                          elem_(size_.width * size_.height * simdWidth),
                                                          mask_(size_.width * size_.height) {
    for (int i = 0; i < size_.width * size_.height; ++i) {
        auto val = image.at<cv::Vec4b>(i);
        mask_[i] = val[3];  // Alpha channel
        for (int j = 0; j < simdWidth; ++j)
            elem_[simdWidth * i + j] = val[2];  // Red channel
    }

    cX_ = (size_.width - 1) / 2;
    cY_ = (size_.height - 1) / 2;
}

std::ostream &operator<<(std::ostream &os, StrEl &el) {
    os << "Structural Element [center: (" << (el.cX_ + 1) << "," << (el.cY_ + 1)
       << "), mult: " << el.simdWidth_ << "]\n";
    os << "--- Weights ---\n";
    for (int y = el.yMin(); y <= el.yMax(); ++y) {
        for (int x = el.xMin(); x <= el.xMax(); ++x)
            os << std::setw(3) << static_cast<int>(el.at(x, y)) << ' ';
        os << '\n';
    }
    os << "--- Mask ---\n";
    for (int y = el.yMin(); y <= el.yMax(); ++y) {
        for (int x = el.xMin(); x <= el.xMax(); ++x)
            os << std::setw(3) << static_cast<int>(el.isSetFuzzy(x, y)) << ' ';
        os << '\n';
    }
    return os;
}
