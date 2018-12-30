#include "morphology/StrEl.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::string thisPath(argv[0]);
        std::cerr << "Usage: " << thisPath.substr(thisPath.find_last_of("/\\") + 1)
                  << " <operation> <image path> <structural element image path>\n";
        return EXIT_SUCCESS;
    }

    try {
        cv::Mat elemMat = cv::imread(argv[3], cv::IMREAD_UNCHANGED);
        if (!elemMat.data)
            throw std::runtime_error("Cannot load structural element image");

        StrEl elem(elemMat);
        std::cout << "Elem:\n" << elem << std::endl;

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
