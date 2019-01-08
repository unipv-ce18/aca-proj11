#include "morphology/StrEl.h"
#include "morphology/operators.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>

const int OP_UNDEFINED = 0;
const int OP_DILATE = 1;
const int OP_ERODE = 2;

static int parseModeArgument(const char *opName) {
    std::string opMode{opName};
    std::transform(opMode.begin(), opMode.end(), opMode.begin(), ::tolower);

    if (opMode == "dilate")
        return OP_DILATE;
    if (opMode == "erode")
        return OP_ERODE;

    return OP_UNDEFINED;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::string thisPath(argv[0]);
        std::cerr << "Usage: " << thisPath.substr(thisPath.find_last_of("/\\") + 1)
                  << " <operation> <structural element path> <image path> [<output path>]\n";
        return EXIT_SUCCESS;
    }

    try {
        int opMode = parseModeArgument(argv[1]);
        if (opMode == OP_UNDEFINED) {
            std::ostringstream msg;
            msg << "Unknown mode: \"" << argv[1] << '"';
            throw std::runtime_error(msg.str());
        }

        cv::Mat elemMat = cv::imread(argv[2], cv::IMREAD_UNCHANGED);
        if (!elemMat.data)
            throw std::runtime_error("Cannot load structural element image");
        StrEl elem(elemMat);

        cv::Mat image = cv::imread(argv[3], cv::IMREAD_GRAYSCALE);
        if (!image.data)
            throw std::runtime_error("Cannot load image to process");


        cv::Mat output;
        auto timeStart = std::chrono::steady_clock::now();

        switch (opMode) {
            case OP_DILATE:
                output = morph::dilate(image, elem);
                break;

            case OP_ERODE:
                // TODO: Implement
                break;

            default:
                throw std::runtime_error("Mode not implemented");
        }

        auto timeEnd = std::chrono::steady_clock::now();
        std::cerr << "Done in "
                  << std::chrono::duration<double, std::milli>(timeEnd - timeStart).count() << "ms" << std::endl;

        if (argc < 5) {
            // Display image if no output parameter is given
            cv::namedWindow("Output image", cv::WINDOW_AUTOSIZE);
            cv::imshow("Output image", output);
            cv::waitKey(0);

        } else {
            cv::imwrite(argv[4], output);
        }

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
