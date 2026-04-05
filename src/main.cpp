#include <opencv2/opencv.hpp>
#include <iostream>
#include "phantom_generator.hpp"
#include "radon.hpp"

int main() {

    cv::Mat img = generatePhantom(256);

    if (img.empty()) {
        std::cerr << "Nie można wczytać obrazu!" << std::endl;
        return -1;
    }
    float policzone = bresenhamSumOfPixels(img, 0, 0, 255, 255);
    std::cout << "Suma pikseli (Bresenham): " << policzone << std::endl;


    cv::imshow("Obraz", img);

    cv::Mat sinogram = radonTransform(img, 1.f, 80, 256.0f);
    cv::Mat sinogram_view;
    cv::normalize(sinogram, sinogram_view, 0.0, 255.0, cv::NORM_MINMAX);
    sinogram_view.convertTo(sinogram_view, CV_8U);
    cv::imshow("Sinogram", sinogram_view);

    cv::Mat reconstructed = inverseRadonTransform(sinogram, 3.f, 256);
    cv::Mat reconstructed_view;
    cv::normalize(reconstructed, reconstructed_view, 0.0, 255.0, cv::NORM_MINMAX);
    reconstructed_view.convertTo(reconstructed_view, CV_8U);
    cv::imshow("Rekonstrukcja", reconstructed_view);

    cv::waitKey(0);
    return 0;
}
