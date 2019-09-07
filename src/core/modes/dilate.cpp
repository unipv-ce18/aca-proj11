#include "dilate.h"

#include "../../parallel/planners.h"
#include "../../morphology/StrEl.h"
#include "../../morphology/operators.h"
#include "../../morphology/dilate_parallel.h"

#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <iostream>

void dilateProc(int argc, char *argv[]) {
    if (argc < 4) {
        throw std::invalid_argument("Not enough arguments");
    }

    cv::Mat elemMat = cv::imread(argv[2], cv::IMREAD_UNCHANGED);
    if (!elemMat.data)
        throw std::runtime_error("Cannot load structural element image");
    StrEl elem(elemMat);

    cv::Mat image = cv::imread(argv[3], cv::IMREAD_GRAYSCALE);
    if (!image.data)
        throw std::runtime_error("Cannot load image to process");

    parallel::Plan plan = parallel::planSimdExecution(4, image.size().width, image.size().height, (elem.size().width-1)/2);

    cv::Mat output;
    auto timeStart = std::chrono::steady_clock::now();

    //output = morph::dilate(image, elem);
    output = dilateParallel(plan, image, elem);

    auto timeEnd = std::chrono::steady_clock::now();
    std::cerr << "Done in "
              << std::chrono::duration<double, std::milli>(timeEnd - timeStart).count() << "ms" << std::endl;

    if (argc < 5) {
        // Display image if no output parameter is given
        cv::namedWindow("Output image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Output image", output);
        cv::waitKey(0);

    } else {
        cv::imwrite(argv[4], output);
    }
}
