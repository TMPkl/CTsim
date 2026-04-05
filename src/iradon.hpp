#ifndef IRADON_HPP
#define IRADON_HPP
#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat inverseRadonTransform(const cv::Mat& sinogram,
                             float delta_angle,
                             int output_size);

std::vector<cv::Point> endpoints(float theta, float d, int image_size);

std::vector<cv::Point> bresenhamIterator(cv::Point start, cv::Point end);
                
#endif // IRADON_HPP