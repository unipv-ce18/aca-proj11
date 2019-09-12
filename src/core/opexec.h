#ifndef MORPH_OPEXEC_H
#define MORPH_OPEXEC_H

#define EXOP_DILATE             1
#define EXOP_ERODE              2
#define EXOP_SKELETON           3
#define EXOP_DILATE_REFERENCE   11

#include <opencv2/core/mat.hpp>
namespace parallel { class Plan; }
class StrEl;

/**
 * Executes the specified morphological using a pre-computed parallel execution plan
 *
 * @param[in]   op              One of EXOP_* indicating the operator to execute
 * @param[in]   plan            The execution plan to actuate
 * @param[out]  output          The output image
 * @param[in]   image           The source image (can be modified in place to preserve memory)
 * @param[in]   elem            The structural element
 * @param[in]   noSimd          Whether or not SIMD processing should be used
 * @param[out]  runTimeMillis   The execution time in milliseconds
 */
void executeOp(int op, parallel::Plan &plan, cv::Mat &output, cv::Mat &image, StrEl &elem,
               bool noSimd, double &runTimeMillis);

#endif //MORPH_OPEXEC_H
