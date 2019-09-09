#ifndef MORPH_OPERATOR_TYPES_H
#define MORPH_OPERATOR_TYPES_H

#include "StrEl.h"

#include <opencv2/core/mat.hpp>


/// Performs the dilation morphological operation
struct Dilate;

/// Performs the erosion morhpological operator
struct Erode;

/// Performs max(skel, I - dilate(nxI, SE)) to ease iterations for the skeleton operator
struct SkelIter;


/// Kernel parameters struct for the base operators (Dilate, Erode)
struct BaseOpParams {
    cv::Mat &out;
    cv::Mat const &src;
    StrEl const &strEl;
};

/// Kernel parameters struct for the Skeleton Iteration operator
struct SkelIterParams {
    cv::Mat &skel;
    cv::Mat const &nxImg;
    StrEl const &strEl;
    cv::Mat const &img;
};



/* Following we have type mappings from operator to its params struct */

template<class T>
struct KernelParams;

template<>
struct KernelParams<Dilate> { using type = BaseOpParams; };

template<>
struct KernelParams<Erode> { using type = BaseOpParams; };

template<>
struct KernelParams<SkelIter> { using type = SkelIterParams; };

#endif //MORPH_OPERATOR_TYPES_H
