#include "dump_plan.h"

#include "parallel/planners.h"
#include "parallel/ostream_dump.h"

#include <iostream>
#include <string>

#define DUMP_PLAN_SIMD_WIDTH 32

void dumpPlanProc(int argc, char **argv) {
    if (argc < 6) {
        throw std::invalid_argument("Not enough arguments");
    }

    try {
        int imgW = std::stoi(argv[2]);
        int imgH = std::stoi(argv[3]);
        int sap = std::stoi(argv[4]);
        int cores = std::stoi(argv[5]);

        std::cerr << "Planning for " << imgW << 'x' << imgH << " (sap " << sap << "), " << cores << " cores\n";
        parallel::Plan p = parallel::planSimdExecution(cores, DUMP_PLAN_SIMD_WIDTH, imgW, imgH, sap);

        using namespace parallel::print_format_json;
        std::cout << p << std::endl;

    } catch (const std::invalid_argument &ex) {
        throw std::invalid_argument("Plan dump argument must be a number");
    }
}
