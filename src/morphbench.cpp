#include "capabilities.h"
#include "simd_props.h"
#include "core/opexec.h"
#include "morphology/StrEl.h"
#include "parallel/planners.h"

#include <opencv2/core.hpp>

#include <numeric>
#include <iostream>

#define BLOCK_WIDTH_NO_SIMD 32

#define IMAGE_WIDTH     1280
#define IMAGE_HEIGHT    1024
#define STREL_SIZE      9

#define WARMUP_ROUNDS_DEFAULT   4

static int getModeId(std::string &modeStr) {
    if (modeStr == "dilate") return EXOP_DILATE;
    if (modeStr == "erode") return EXOP_ERODE;
    if (modeStr == "skeleton") return EXOP_SKELETON;
    throw std::invalid_argument("Unknown mode");
}

template<typename T>
static T computeStdDev(std::vector<T> &times, T mean) {
    double varianceAccum = 0;
    for (auto t : times) varianceAccum += std::pow(t - mean, 2);
    return std::sqrt(varianceAccum / times.size());
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <operation> <rounds>\n\n"
        << "Available modes:\n"
           "  (same as morph, excl. dumpPlan)\n\n"
        << "Environment variables:\n"
        << "  MORPHBENCH_WARMUP_ROUNDS\n"
           "    Number of warmup rounds before starting the benchmark\n"
           "  (morph variables can be used in morphbench too)";
        return EXIT_SUCCESS;
    }

    std::string modeStr(argv[1]);
    std::transform(modeStr.begin(), modeStr.end(), modeStr.begin(), ::tolower);

    const char *wrStr = std::getenv("MORPHBENCH_WARMUP_ROUNDS");

    const int mode = getModeId(modeStr);
    const int warmupRounds = wrStr != nullptr ? std::atoi(wrStr) : WARMUP_ROUNDS_DEFAULT;
    const int rounds = std::atoi(argv[2]);

    cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    cv::randu(image, cv::Scalar(0), cv::Scalar(255));

    cv::Mat strElImg(STREL_SIZE, STREL_SIZE, CV_8UC4);
    cv::randu(strElImg, cv::Scalar(0, 0, 0, 0), cv::Scalar(0, 0, 255, 255));
    StrEl elem(strElImg);

    // TODO copy-paste from operators.cpp, ROFL
    ParallelConfig conf = makeParallelConfig();
    int planSimdWidth = conf.simdWidth == DEFAULT_BLOCK_WIDTH ? BLOCK_WIDTH_NO_SIMD : conf.simdWidth;
    std::cerr << "Using " << conf.numCores << " cores (" << conf.simdWidth << " simd pixels)\n";
    parallel::Plan plan = parallel::planSimdExecution(conf.numCores, planSimdWidth,
                                                      image.size().width, image.size().height,
                                                      (elem.size().width - 1) / 2);

    bool noSimd = conf.simdWidth == DEFAULT_BLOCK_WIDTH;

    cv::Mat output(image.size(), CV_8UC1);

    for (int r = 0; r < warmupRounds; ++r) {
        std::cerr << "Warming up... (" << r + 1 << '/' << warmupRounds << ")\r";

        double dummyTime = 0;
        executeOp(mode, plan, output, image, elem, noSimd, dummyTime);
    }
    if (warmupRounds > 0) std::cerr << std::endl;

    std::vector<double> times(rounds);

    for (int r = 0; r < rounds; ++r) {
        std::cerr << "Benchmarking... (" << r + 1 << '/' << rounds;
        if (r > 0) std::cerr << ", " << times[r - 1] << "ms";
        std::cerr << ")\r";

        executeOp(EXOP_DILATE, plan, output, image, elem, noSimd, times[r]);
    }
    std::cerr << std::endl;

    auto timeMin = std::min_element(times.begin(), times.end());
    auto timeMax = std::max_element(times.begin(), times.end());
    double timeTot = std::accumulate(times.begin(), times.end(), 0.0);
    double timeAvg = timeTot / times.size();

    std::cerr << "Completed in " << timeTot << "ms ("
              << "min: " << *timeMin << "ms, max: " << *timeMax << "ms, "
              << "avg: " << timeAvg << "ms, dev: " << computeStdDev(times, timeAvg) << "ms)\n";

    return EXIT_SUCCESS;
}
