#ifndef MORPH_PLANNERS_H
#define MORPH_PLANNERS_H

#include "Plan.h"

/**
 * Algorithms for planning parallel execution of \ref index "Morph's" algorithms.
 */
namespace parallel {

    /**
     * Generates a parallel execution plan optimized for the enabled SIMD processor extensions.
     *
     * @param cores The number of cores to plan for
     * @param imgW The image width
     * @param imgH The image height
     * @param sap Distance from the border from where the execution is safe
     * @return The built execution plan
     */
    Plan planSimdExecution(int cores, int imgW, int imgH, int sap);

}

#endif //MORPH_PLANNERS_H
