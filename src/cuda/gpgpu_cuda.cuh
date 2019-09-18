#ifndef ACA_PROJECT_GPGPU_CUDA_H
#define ACA_PROJECT_GPGPU_CUDA_H

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <opencv2/core/mat.hpp>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort = true) {
    if (code != cudaSuccess) {
        fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort) exit(code);
    }
}

class StrEl;

namespace morphcuda {

    struct GridConfig {
        dim3 gridBlocksSz;
        dim3 blockThreadsSz;
    };

    cv::Mat dilate(const GridConfig &gridConf, cv::Mat &img, StrEl &strEl);
}

#endif //ACA_PROJECT_GPGPU_CUDA_H
