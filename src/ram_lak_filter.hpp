#ifndef RAM_LAK_FILTER_HPP
#define RAM_LAK_FILTER_HPP

#include <opencv2/opencv.hpp>

/**
 * Zastosuj filtr Ram-Lak (Ramp filter) do sinogramu
 * @param sinogram Sinogram wejściowy (CV_32F)
 * @return Sinogram przefiltrowany filtrem Ram-Lak (CV_32F)
 */
cv::Mat applyRamLakFilter(const cv::Mat& sinogram);

#endif // RAM_LAK_FILTER_HPP
