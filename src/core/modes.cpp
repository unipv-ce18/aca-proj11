#include "modes.h"
#include "modes/operators.h"
#include "modes/dump_plan.h"

#include <iostream>
#include <string>
#include <stdexcept>


inline std::string extractFileName(const std::string &path) {
    return path.substr(path.find_last_of("/\\") + 1);
}

ModeProc getRunMode(const char *modeStr) {
    if (modeStr == nullptr)
        throw std::invalid_argument("Mode string is null");

    const std::string &mode{modeStr};
    if (mode == "dilate") return dilateProc;
    if (mode == "erode") return erodeProc;
    if (mode == "skeleton") return skeletonProc;
    if (mode == "dumpplan") return dumpPlanProc;

    return nullptr;
}

void printUsage(const char *execStr) {
    std::cerr << "Usage: " << extractFileName(execStr) << " <operation> [mode args]\n\n"
              << "Available modes:\n"
              << "  dilate <structural element path> <image path> [<output path>]\n"
                 "    Performs dilation on a given image\n"
              << "  erode <structural element path> <image path> [<output path>]\n"
                 "    Performs erosion on a given image\n"
              << "  skeleton <structural element path> <image path> [<output path>]\n"
                 "    Performs skeleton on a given image\n"
              << "  dumpPlan <width> <height> <safe padding> <cores>\n"
                 "    Dumps an execution plan for the given parameters in JSON format\n"
              << "\n"
              << "Environment variables:\n"
              << "  MORPH_THREADS\n"
                 "The number of threads to execute (defaults to number of CPU cores)\n"
              << "MORPH_SIMD\n"
                 "One of \"SSE2\", \"AVX2\", \"AVX512\" or \"none\" for kernel selection\n";
}
