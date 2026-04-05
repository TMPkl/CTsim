#ifndef RADON_HPP
#define RADON_HPP

#include <opencv2/opencv.hpp>

cv::Mat radonTransform(const cv::Mat& img,
                       float delta_angle,
                       int number_of_emitters,
                       float detector_span);

float bresenhamSumOfPixels(const cv::Mat& img, int x0, int y0, int x1, int y1);
float bresenhamMeanOfPixels(const cv::Mat& img, int x0, int y0, int x1, int y1);
cv::Mat inverseRadonTransform(const cv::Mat& sinogram,
                             float delta_angle,
                             int output_size);

#endif // RADON_HPP