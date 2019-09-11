#include "operators.h"

#include "capabilities.h"
#include "core/opexec.h"
#include "parallel/planners.h"
#include "morphology/StrEl.h"
#include "simd_props.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>

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
    const bool noSimd = conf.simdWidth == SIMD_WIDTH_NO_SIMD;
    const int safePadding = (elem.size().width - 1) / 2;
    parallel::Plan plan = parallel::planSimdExecution(conf.numCores, noSimd ? BLOCK_WIDTH_NO_SIMD : conf.simdWidth,
                                                      image.size().width, image.size().height, safePadding);

    std::cerr << "Using " << conf.numCores << " cores (";
    if (noSimd) std::cerr << "no simd)\n";
    else std::cerr << conf.simdWidth << " simd pixels)\n";

    cv::Mat output(image.size(), CV_8UC1);

    double runTimeMillis;
    executeOp(op, plan, output, image, elem, noSimd, runTimeMillis);
    std::cerr << "Done in " << runTimeMillis << "ms" << std::endl;

    image.release();

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
    operProcCommon(EXOP_DILATE, argc, argv);
}

void erodeProc(int argc, char **argv) {
    operProcCommon(EXOP_ERODE, argc, argv);
}
void skeletonProc(int argc, char **argv) {
    operProcCommon(EXOP_SKELETON, argc, argv);
}
