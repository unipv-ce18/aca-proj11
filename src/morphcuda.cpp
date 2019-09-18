#include "cuda/gpgpu_cuda.cuh"
#include "morphology/StrEl.h"

#include <cuda_runtime.h>
#include <opencv2/highgui.hpp>

#include <stdexcept>
#include <iostream>
#include <string>

morphcuda::GridConfig calcGridConfiguration(const cv::Size &imSize) {
    int device;
    gpuErrchk(cudaGetDevice(&device));

    // Assume we use only the first available device, no cudaGetDeviceCount()
    cudaDeviceProp deviceProps;
    cudaGetDeviceProperties(&deviceProps, device);

    std::cerr << "CUDA Device: " << deviceProps.name << '\n'
              << "  SM count:       " << deviceProps.multiProcessorCount << '\n'
              << "    Max threads:  " << deviceProps.maxThreadsPerMultiProcessor << " each\n"
              << "    Warp size:    " << deviceProps.warpSize << " threads\n"
              << "  Max grid dims:  "
              << deviceProps.maxGridSize[0] << " x " << deviceProps.maxGridSize[1] << " x "
              << deviceProps.maxGridSize[2]
              << " blocks\n"
              << "  Max block dims: "
              << deviceProps.maxThreadsDim[0] << " x " << deviceProps.maxThreadsDim[1] << " x "
              << deviceProps.maxThreadsDim[2] << " threads\n"
              << "    Max threads:  " << deviceProps.maxThreadsPerBlock << " per block\n\n";

    uint32_t thrSz = 1;
    while ((thrSz << 1) * (thrSz << 1) <= deviceProps.maxThreadsPerBlock) thrSz <<= 1;
    uint32_t grW = static_cast<int>(std::ceil(static_cast<float>(imSize.width) / thrSz));
    uint32_t grH = static_cast<int>(std::ceil(static_cast<float>(imSize.height) / thrSz));

    std::cout << "Using a " << grW << " x " << grH << " grid with "
              << thrSz << " x " << thrSz << " threads per block\n";

    return {{grW,   grH},
            {thrSz, thrSz}};
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <operation> <structural element path> <image path> [<output path>]\n";
        return EXIT_FAILURE;
    }

    std::string opStr(argv[1]);
    std::transform(opStr.begin(), opStr.end(), opStr.begin(), ::tolower);
    if (opStr != "dilate") {
        std::cerr << "Sorry, only dilation is implemented right now";
        return EXIT_FAILURE;
    }

    try {
        cv::Mat elemMat = cv::imread(argv[2], cv::IMREAD_UNCHANGED);
        cv::Mat image = cv::imread(argv[3], cv::IMREAD_GRAYSCALE);
        StrEl elem(elemMat, 1);

        cv::Mat out = morphcuda::dilate(calcGridConfiguration(image.size()), image, elem);

        if (argc < 5) {
            // Display image if no output parameter is given
            cv::namedWindow("Output image", cv::WINDOW_AUTOSIZE);
            cv::imshow("Output image", out);
            cv::waitKey(0);

        } else {
            cv::imwrite(argv[4], out);
        }

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
