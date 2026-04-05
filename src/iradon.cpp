#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include "iradon.hpp"

static cv::Mat applyRamLakFilter(const cv::Mat& sinogram) {
    CV_Assert(sinogram.type() == CV_32F);

    int num_emitters = sinogram.rows;
    int num_angles = sinogram.cols;
    int n = cv::getOptimalDFTSize(num_emitters);

    cv::Mat filtered = cv::Mat::zeros(num_emitters, num_angles, CV_32F);

    for (int a = 0; a < num_angles; ++a) {
        cv::Mat padded = cv::Mat::zeros(n, 1, CV_32F);
        sinogram.col(a).copyTo(padded.rowRange(0, num_emitters));

        cv::Mat planes[] = {padded, cv::Mat::zeros(n, 1, CV_32F)};
        cv::Mat spectrum;
        cv::merge(planes, 2, spectrum);
        cv::dft(spectrum, spectrum);

        for (int k = 0; k < n; ++k) {
            float freq = (k <= n / 2) ? static_cast<float>(k) : static_cast<float>(n - k);
            float ramp = freq / static_cast<float>(std::max(1, n / 2));

            cv::Vec2f& bin = spectrum.at<cv::Vec2f>(k, 0);
            bin[0] *= ramp;
            bin[1] *= ramp;
        }

        cv::Mat inv;
        cv::idft(spectrum, inv, cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
        inv.rowRange(0, num_emitters).copyTo(filtered.col(a));
    }

    return filtered;
}

std::vector<cv::Point> bresenhamIterator(cv::Point start, cv::Point end){
    double dx = std::abs(end.x - start.x);
    double dy = std::abs(end.y - start.y);

    int sx = (start.x < end.x) ? 1 : -1;
    int sy = (start.y < end.y) ? 1 : -1;

    double err = dx - dy;

    std::vector<cv::Point> points;

    while(true) {
        points.push_back(start);

        if (start.x == end.x && start.y == end.y)
            break;

        double e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            start.y += sy;
        }
    }

    return points;
}

std::vector<cv::Point> endpoints(float theta, float d, int image_size) {
    float cos_t = std::cos(theta);
    float sin_t = std::sin(theta);

    int x0 = image_size / 2 + static_cast<int>(std::lround(d * cos_t));
    int y0 = image_size / 2 + static_cast<int>(std::lround(d * sin_t));

    float half_ray = image_size / 2.0f;

    int x1 = x0 + static_cast<int>(std::lround(half_ray * (-sin_t)));
    int y1 = y0 + static_cast<int>(std::lround(half_ray * ( cos_t)));

    return {cv::Point(x1, y1), cv::Point(x0 - static_cast<int>(std::lround(half_ray * (-sin_t))),
                                         y0 - static_cast<int>(std::lround(half_ray * ( cos_t))))};
}

cv::Mat inverseRadonTransform(const cv::Mat& sinogram,
                             float delta_angle,
                             int output_size) {
    cv::Mat reconstructed = cv::Mat::zeros(output_size, output_size, CV_32F);
    cv::Mat coverage = cv::Mat::zeros(output_size, output_size, CV_32F);

    int num_angles = sinogram.cols;
    int num_emitters = sinogram.rows;
    float detector_span = static_cast<float>(output_size);
    float detector_step = detector_span / static_cast<float>(std::max(1, num_emitters - 1));

    for (int a = 0; a < num_angles; ++a) {
        float theta = a * delta_angle * CV_PI / 180.0f;

        for (int i = 0; i < num_emitters; ++i) {
            float d = -detector_span / 2.0f + i * detector_step;
            auto start_end_points = endpoints(theta, d, output_size);
                cv::Point start = start_end_points[0];
                cv::Point end = start_end_points[1];

            std::vector<cv::Point> line_points = bresenhamIterator(start, end);
            for (const auto& pt : line_points) {
                if (pt.x >= 0 && pt.x < output_size && pt.y >= 0 && pt.y < output_size) {
                    reconstructed.at<float>(pt.y, pt.x) += sinogram.at<float>(i, a);
                    coverage.at<float>(pt.y, pt.x) += 1.0f;
                }
            }
        }
    }

    for (int y = 0; y < output_size; ++y) {
        for (int x = 0; x < output_size; ++x) {
            float hits = coverage.at<float>(y, x);
            if (hits > 0.0f) {
                reconstructed.at<float>(y, x) /= hits;
            }
        }
    }

    return reconstructed;
}