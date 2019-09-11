#include "capabilities.h"
#include "simd_props.h"
#include "core/opexec.h"
#include "morphology/StrEl.h"
#include "parallel/planners.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <numeric>
#include <iostream>

#define ENV_KEY_IMAGE   "MORPHBENCH_IMAGE"
#define ENV_KEY_STREL   "MORPHBENCH_STREL"
#define ENV_KEY_WARMUP  "MORPHBENCH_WARMUP_ROUNDS"

#define DEF_IMG_W       1280
#define DEF_IMG_H       1024
#define DEF_STREL_SZ    9

#define WARMUP_ROUNDS_DEFAULT   4

#define BLOCK_WIDTH_NO_SIMD 32

struct BenchEnvParams {
    int mode;
    int warmupRounds;
    int benchRounds;
    cv::Mat image;
    StrEl elem;
};

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

static BenchEnvParams getEnvParams(char *argv[]) {
    const char *envImage = std::getenv(ENV_KEY_IMAGE);
    const char *envStrEl = std::getenv(ENV_KEY_STREL);
    const char *wrStr = std::getenv(ENV_KEY_WARMUP);

    std::string modeStr(argv[1]);
    std::transform(modeStr.begin(), modeStr.end(), modeStr.begin(), ::tolower);
    const int mode = getModeId(modeStr);

    const int rounds = std::atoi(argv[2]);

    cv::Mat img;
    if (envImage == nullptr) {
        img = cv::Mat(DEF_IMG_H, DEF_IMG_W, CV_8UC1);
        cv::randu(img, cv::Scalar(0), cv::Scalar(255));
        std::cerr << "Image: [randomly generated, " << DEF_IMG_W << 'x' << DEF_IMG_H << "]\n";
    } else {
        img = cv::imread(envImage, cv::IMREAD_GRAYSCALE);
        std::cerr << "Image: \"" << envImage << "\"\n";
    }

    cv::Mat strElImg;
    if (envStrEl == nullptr) {
        strElImg = cv::Mat(DEF_STREL_SZ, DEF_STREL_SZ, CV_8UC4);
        cv::randu(strElImg, cv::Scalar(0, 0, 0, 0), cv::Scalar(0, 0, 255, 255));
        std::cerr << "StrEl: [randomly generated, size " << DEF_STREL_SZ << "]\n";
    } else {
        strElImg = cv::imread(envStrEl, cv::IMREAD_UNCHANGED);
        std::cerr << "StrEl: \"" << envStrEl << "\"\n";
    }

    const int warmupRounds = wrStr != nullptr ? std::atoi(wrStr) : WARMUP_ROUNDS_DEFAULT;

    return {mode, warmupRounds, rounds, std::move(img), StrEl(strElImg)};
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr
            << "Usage: " << argv[0] << " <operation> <rounds>\n\n"
            << "Available modes:\n"
               "  (same as morph, excl. dumpPlan)\n\n"
            << "Environment variables:\n"
            << "  " ENV_KEY_IMAGE "\n"
               "    The image to process (default " << DEF_IMG_W << 'x' << DEF_IMG_H << " random)\n"
            << "  " ENV_KEY_STREL "\n"
               "    The structural element to use (default random of size " << DEF_STREL_SZ << ")\n"
            << "  " ENV_KEY_WARMUP "\n"
               "    Number of warmup rounds before starting the benchmark (default " << WARMUP_ROUNDS_DEFAULT << ")\n"
               "  (morph variables can be used in morphbench too)";
        return EXIT_SUCCESS;
    }

    BenchEnvParams par = getEnvParams(argv);

    // TODO copy-paste from operators.cpp, ROFL
    ParallelConfig conf = makeParallelConfig();
    const bool noSimd = conf.simdWidth == SIMD_WIDTH_NO_SIMD;
    const int safePadding = (par.elem.size().width - 1) / 2;
    parallel::Plan plan = parallel::planSimdExecution(conf.numCores, noSimd ? BLOCK_WIDTH_NO_SIMD : conf.simdWidth,
                                                      par.image.size().width, par.image.size().height, safePadding);

    std::cerr << "Using " << conf.numCores << " cores (";
    if (noSimd) std::cerr << "no simd)\n";
    else std::cerr << conf.simdWidth << " simd pixels)\n";

    cv::Mat output(par.image.size(), CV_8UC1);
    std::vector<double> times(par.benchRounds);

    for (int r = 0; r < par.warmupRounds; ++r) {
        std::cerr << "Warming up... (" << r + 1 << '/' << par.warmupRounds << ")\r";
        double dummyTime = 0;
        executeOp(par.mode, plan, output, par.image, par.elem, noSimd, dummyTime);
    }
    if (par.warmupRounds > 0) std::cerr << std::endl;

    for (int r = 0; r < par.benchRounds; ++r) {
        std::cerr << "Benchmarking... (" << r + 1 << '/' << par.benchRounds;
        if (r > 0) std::cerr << ", " << times[r - 1] << "ms";
        std::cerr << ")\r";
        executeOp(EXOP_DILATE, plan, output, par.image, par.elem, noSimd, times[r]);
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
