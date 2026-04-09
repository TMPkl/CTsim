#include <opencv2/opencv.hpp>
#include <iostream>
#include "phantom_generator.hpp"
#include "radon.hpp"

#define USE_MASK 1
#define USE_MEDIAN_BLUR 1
#define MEDIAN_BLUR_KERNEL_SIZE 5

int main() {

    cv::Mat img = generatePhantom(256);

    if (img.empty()) {
        std::cerr << "Nie można wczytać obrazu!" << std::endl;
        return -1;
    }
    // float policzone = bresenhamSumOfPixels(img, 0, 0, 255, 255);
    // std::cout << "Suma pikseli (Bresenham): " << policzone << std::endl;
    

    cv::imshow("Obraz", img);

    cv::Mat sinogram = radonTransform(img, 0.1f, 300, 255.0f);
    cv::Mat sinogram_view;
    cv::normalize(sinogram, sinogram_view, 0.0, 255.0, cv::NORM_MINMAX);
    sinogram_view.convertTo(sinogram_view, CV_8U);
    cv::imshow("Sinogram", sinogram_view);

    cv::Mat reconstructed = inverseRadonTransform(sinogram, 0.1f, 256);
    cv::Mat reconstructed_view;
    
    if(USE_MASK) {
        cv::Mat mask = cv::Mat::zeros(reconstructed.size(), CV_8U);
        cv::circle(mask, {reconstructed.cols/2, reconstructed.rows/2}, reconstructed.cols/2, 255, -1);
        cv::normalize(reconstructed, reconstructed_view, 0.0, 255.0, cv::NORM_MINMAX,-1, mask );

    } else {
        cv::normalize(reconstructed, reconstructed_view, 0.0, 255.0, cv::NORM_MINMAX);
    }
        reconstructed_view.convertTo(reconstructed_view, CV_8U);
    if(USE_MEDIAN_BLUR) {
        cv::medianBlur(reconstructed_view, reconstructed_view, MEDIAN_BLUR_KERNEL_SIZE);
    }
    

    cv::imshow("Rekonstrukcja", reconstructed_view);

    cv::waitKey(0);
    return 0;
}
