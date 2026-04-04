#include <opencv2/opencv.hpp>
#include <iostream>
#include "phantom_generator.hpp"

int main() {

    cv::Mat img = generatePhantom(256);

    if (img.empty()) {
        std::cerr << "Nie można wczytać obrazu!" << std::endl;
        return -1;
    }


    cv::imshow("Obraz", img);
    cv::waitKey(0);

    return 0;
}
