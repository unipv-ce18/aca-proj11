#include "operators.h"

#include "StrEl.h"

#include <opencv2/core/core.hpp>
#include <iostream>

cv::Mat morph::dilate(const cv::Mat &image, const StrEl &strEl) {
    assert(image.type() == CV_8UC1);

    cv::Size imSize = image.size();
    cv::Mat output(imSize, CV_8UC1);

    for (int y = 0; y < imSize.height; ++y) {
        for (int x = 0; x < imSize.width; ++x) {

            int val = 0;

            for (int j = strEl.yMin(); j <= strEl.yMax(); ++j) {
                for (int i = strEl.xMin(); i <= strEl.xMax(); ++i) {
                    int u = x + i;
                    int v = y + j;

                    if (v < 0 || v >= imSize.height) continue;
                    if (u < 0 || u >= imSize.width) continue;
                    if (!strEl.isSet(j, i)) continue;

                    int m = image.at<uint8_t>(v, u) + strEl.at(j, i);
                    if (m > val) val = m;
                }
            }
            output.at<uint8_t>(y, x) = static_cast<uint8_t>(val < 0xFF ? val : 0xFF);
        }
    }

    return output;
}
