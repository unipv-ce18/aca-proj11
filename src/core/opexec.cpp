#include "opexec.h"

#include "morphology/reference.h"
#include "morphology/operator_types.h"
#include "morphology/process_parallel.h"
#include "parallel/Plan.h"

#include "opencv2/core.hpp"

#include <chrono>

void executeOp(int op, parallel::Plan &plan, cv::Mat &output, cv::Mat &image, const StrEl &elem,
               bool noSimd, double &runTimeMillis) {
    auto timeStart = std::chrono::steady_clock::now();

    switch (op) {
        case EXOP_DILATE:
            processParallel<Dilate>(plan, { output, image, elem }, noSimd);
            break;
        case EXOP_DILATE_REFERENCE:
            image = morph::dilate(image, elem, plan.cores());
            break;
        case EXOP_ERODE:
            processParallel<Erode>(plan, { output, image, elem }, noSimd);
            break;
        case EXOP_SKELETON: {
            cv::Mat temp(image.size(),CV_8UC1);

            cv::Mat *I = &image;
            cv::Mat *nxI = &temp;

            //zero fill the skeleton first
            output.setTo(0);

            int count;
            while ((count = cv::countNonZero(*I)) > 0) {
                processParallel<Erode>(plan, {*nxI, *I, elem}, noSimd);
                processParallel<SkelIter>(plan, {output, *nxI, elem, *I}, noSimd);
                std::swap(I, nxI);
                //std::cerr << "[Skeleton] " << count << " pixels remaining\n";
            }
            temp.release();
            break;
        }
        default:
            throw std::runtime_error("Unknown internal operation mode");
    }

    auto timeEnd = std::chrono::steady_clock::now();

    runTimeMillis = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
}
