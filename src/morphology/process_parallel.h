#ifndef MORPH_PROCESS_PARALLEL_H
#define MORPH_PROCESS_PARALLEL_H

#include "operator_types.h"

#include <opencv2/core/mat.hpp>

// Forward declarations
namespace parallel { class Plan; }
class StrEl;

template<typename Operator, typename OpArgs = typename KernelParams<Operator>::type>
void processParallel(parallel::Plan &plan, OpArgs params, const bool noSimd);

#endif //MORPH_PROCESS_PARALLEL_H
