#include "gpgpu_cuda.cuh"

#include "morphology/StrEl.h"

#include <opencv2/core/mat.hpp>

__global__ void kdilate(uint8_t *pIn, uint8_t *pOut, uint8_t *pSe, uint8_t *pM, int imW, int imH, int seW, cv::Point seC) {
    uint32_t x = (blockIdx.x * blockDim.x) + threadIdx.x;
    uint32_t y = (blockIdx.y * blockDim.y) + threadIdx.y;
    if (y >= imH || x >= imW) return;

    int val = 0;

    for (int j = -seC.y; j <= seC.y; ++j) {
        for (int i = -seC.x; i <= seC.x; ++i) {
            int seIdx = (seC.y + j) * seW + seC.x + i;
            if (pM[seIdx] == 0) continue;
            if (y + j >= imH || x + i >= imW) continue;

            int imIdx = (y + j) * imW + x + i;
            val = max(val, pIn[imIdx] + pSe[seIdx]);
        }
    }

    pOut[y * imW + x] = static_cast<uint8_t>(min(val, 0xFF));
}

cv::Mat morphcuda::dilate(const GridConfig &gridConfig, cv::Mat &img, StrEl &strEl) {
    assert(img.type() == CV_8UC1);

    cudaEvent_t evtStart, evtCopy, evtCompute, evtCopyBack;
    cudaEventCreate(&evtStart);
    cudaEventCreate(&evtCopy);
    cudaEventCreate(&evtCompute);
    cudaEventCreate(&evtCopyBack);

    uint8_t *d_img, *d_out, *d_strel, *d_mask;

    int imgSz = img.rows * img.cols * sizeof(uint8_t);
    int strelSz = strEl.size().width * strEl.size().height * sizeof(uint8_t);    // assuming StrEl has expansion of 1

    cv::Mat out(img.size(), CV_8UC1);

    cudaEventRecord(evtStart);

    // Consider using cudaMallocManaged
    gpuErrchk(cudaMalloc(reinterpret_cast<void **>(&d_img), imgSz));
    gpuErrchk(cudaMalloc(reinterpret_cast<void **>(&d_out), imgSz));
    gpuErrchk(cudaMalloc(reinterpret_cast<void **>(&d_strel), strelSz));
    gpuErrchk(cudaMalloc(reinterpret_cast<void **>(&d_mask), strelSz));

    gpuErrchk(cudaMemcpy(d_img, img.data, imgSz, cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(d_strel, strEl.elemPtr(), strelSz, cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(d_mask, strEl.maskPtr(), strelSz, cudaMemcpyHostToDevice));

    cudaEventRecord(evtCopy);
    kdilate<<<gridConfig.gridBlocksSz, gridConfig.blockThreadsSz>>>(
            d_img, d_out, d_strel, d_mask, img.size().width, img.size().height, strEl.size().width, cv::Point(strEl.xMax(),strEl.yMax()));
    cudaEventRecord(evtCompute);

    gpuErrchk(cudaDeviceSynchronize());

    gpuErrchk(cudaMemcpy(out.data, d_out, imgSz, cudaMemcpyDeviceToHost));

    cudaEventRecord(evtCopyBack);

    gpuErrchk(cudaFree(d_img));
    gpuErrchk(cudaFree(d_out));
    gpuErrchk(cudaFree(d_strel));
    gpuErrchk(cudaFree(d_mask));

    float timeCopy, timeExec, timeCopyBack;
    cudaEventElapsedTime(&timeCopy, evtStart, evtCopy);
    cudaEventElapsedTime(&timeExec, evtCopy, evtCompute);
    cudaEventElapsedTime(&timeCopyBack, evtCompute, evtCopyBack);

    fprintf(stderr, "Done in %fms (%fms copy, %fms exec, %fms copy back)\n",
            timeCopy + timeExec + timeCopyBack, timeCopy, timeExec, timeCopyBack);

    return out;
}
