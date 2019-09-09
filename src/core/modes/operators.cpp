#include "operators.h"

#include "../../capabilities.h"
#include "../../parallel/planners.h"
#include "../../morphology/StrEl.h"
#include "../../morphology/reference.h"
#include "../../morphology/operator_types.h"
#include "../../simd_props.h"

// We need to include this to let this compilation unit generate the right version of the method
#include "../../morphology/process_parallel.tcc"

#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <iostream>

#define OPERATOR_DILATE 1
#define OPERATOR_ERODE  2

#define BLOCK_WIDTH_NO_SIMD 32

static void operProcCommon(int op, int argc, char *argv[]) {
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
    assert(image.type() == CV_8UC1);

    ParallelConfig conf = makeParallelConfig();
    std::cerr << "Using " << conf.numCores << " cores (" << conf.simdWidth << " simd pixels)\n";
    parallel::Plan plan = parallel::planSimdExecution(conf.numCores, conf.simdWidth == DEFAULT_BLOCK_WIDTH ? BLOCK_WIDTH_NO_SIMD : conf.simdWidth,
            image.size().width, image.size().height, (elem.size().width-1)/2);

    cv::Mat output(image.size(), CV_8UC1);

    auto timeStart = std::chrono::steady_clock::now();

    switch (op) {
        case OPERATOR_DILATE:
            //output = morph::dilate(image, elem);
            processParallel<Dilate>(plan, { output, image, elem }, conf.simdWidth == DEFAULT_BLOCK_WIDTH);
            break;
        case OPERATOR_ERODE:
            processParallel<Erode>(plan, { output, image, elem }, conf.simdWidth == DEFAULT_BLOCK_WIDTH);
            break;
        default:
            throw std::runtime_error("Unknown internal operation mode");
    }

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

void dilateProc(int argc, char **argv) {
    operProcCommon(OPERATOR_DILATE, argc, argv);
}

void erodeProc(int argc, char **argv) {
    operProcCommon(OPERATOR_ERODE, argc, argv);
}
