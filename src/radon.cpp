#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include "radon.hpp"

float bresenhamSumOfPixels(const cv::Mat& img, int x0, int y0, int x1, int y1) {
    float sum = 0.0f;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < img.cols && y0 >= 0 && y0 < img.rows) {
            sum += img.at<double>(y0, x0);
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return sum;
}

float bresenhamMeanOfPixels(const cv::Mat& img, int x0, int y0, int x1, int y1) {
    float sum = 0.0f;
    int count = 0;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < img.cols && y0 >= 0 && y0 < img.rows) {
            sum += img.at<double>(y0, x0);
            ++count;
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return (count > 0) ? (sum / static_cast<float>(count)) : 0.0f;
}


cv::Mat radonTransform(const cv::Mat& img,
                       float delta_angle,
                       int number_of_emitters,
                       float detector_span) {

    int width = img.cols;
    int height = img.rows;

    int num_angles = 180 / delta_angle;

    cv::Mat sinogram = cv::Mat::zeros(number_of_emitters, num_angles, CV_32F);

    int centerX = width / 2;
    int centerY = height / 2;

    float half_ray = std::min(width, height) / 2.0f;

    for (int a = 0; a < num_angles; ++a) {
        float theta = a * delta_angle * CV_PI / 180.0f;

        float cos_t = std::cos(theta);
        float sin_t = std::sin(theta);

        for (int i = 0; i < number_of_emitters; ++i) {

            float emitter_step = detector_span / static_cast<float>(std::max(1, number_of_emitters - 1));
            float s = -detector_span / 2.0f + i * emitter_step;

            int x0 = centerX + static_cast<int>(std::lround(s * cos_t));
            int y0 = centerY + static_cast<int>(std::lround(s * sin_t));

            int x1 = x0 + static_cast<int>(std::lround(half_ray * (-sin_t)));
            int y1 = y0 + static_cast<int>(std::lround(half_ray * ( cos_t)));

            int x2 = x0 - static_cast<int>(std::lround(half_ray * (-sin_t)));
            int y2 = y0 - static_cast<int>(std::lround(half_ray * ( cos_t)));

            float mean = bresenhamMeanOfPixels(img, x1, y1, x2, y2);

            sinogram.at<float>(i, a) = mean;
        }
    }

    //cv::medianBlur(sinogram, sinogram, 3);
    //cv::GaussianBlur(sinogram, sinogram, cv::Size(5, 5), 0);
    //cv::GaussianBlur(sinogram, sinogram, cv::Size(7, 7), 0);


    return sinogram;
}
