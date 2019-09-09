#include "core/modes.h"
#include "capabilities.h"

#include <algorithm>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // No mode argument, print usage & exit
        printUsage(argv[0]);
        return EXIT_SUCCESS;
    }

    std::string opMode{argv[1]};
    std::transform(opMode.begin(), opMode.end(), opMode.begin(), ::tolower);

    ModeProc modeProc = nullptr;
    if ((modeProc = getRunMode(opMode.c_str())) == nullptr) {
        std::cerr << "Error: Unknown mode\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    try {
        ParallelConfig conf = makeParallelConfig();
        std::cerr << "Using " << conf.numCores << " cores (" << conf.simdWidth << " simd pixels)\n";
        modeProc(argc, argv);

    } catch (const std::invalid_argument &ex) {
        // Wrong arguments passed in, print usage
        std::cerr << "Error: " << ex.what() << std::endl;
        printUsage(argv[0]);
        return EXIT_FAILURE;

    } catch (const std::exception &ex) {
        // Exit gracefully after generic mode exception
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
