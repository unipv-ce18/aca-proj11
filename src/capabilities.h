#ifndef ACA_PROJECT_CAPABILITIES_H
#define ACA_PROJECT_CAPABILITIES_H

struct ParallelConfig {
    int numCores;
    int simdWidth;
};

ParallelConfig makeParallelConfig();
#endif //ACA_PROJECT_CAPABILITIES_H
