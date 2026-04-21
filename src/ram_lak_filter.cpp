#include <opencv2/opencv.hpp>
#include <cmath>
#include "ram_lak_filter.hpp"

cv::Mat applyRamLakFilter(const cv::Mat& sinogram) {
    CV_Assert(sinogram.type() == CV_32F);

    int num_emitters = sinogram.rows;
    int num_angles = sinogram.cols;
    int n = cv::getOptimalDFTSize(num_emitters);

    cv::Mat filtered = cv::Mat::zeros(num_emitters, num_angles, CV_32F);

    // Przetwarzaj każdy kąt niezależnie
    for (int a = 0; a < num_angles; ++a) {
        // Stwórz wektorpaddowany zerami do optymalnego rozmiaru DFT
        cv::Mat padded = cv::Mat::zeros(n, 1, CV_32F);
        sinogram.col(a).copyTo(padded.rowRange(0, num_emitters));

        // Przygotuj spektrum (liczby zespolone)
        cv::Mat planes[] = {padded, cv::Mat::zeros(n, 1, CV_32F)};
        cv::Mat spectrum;
        cv::merge(planes, 2, spectrum);
        
        // Transformata Fouriera
        cv::dft(spectrum, spectrum);

        // Zastosuj filtr Ram-Lak (ramp filter) w domenie częstotliwości
        for (int k = 0; k < n; ++k) {
            // Liniowy ramp od 0 do 1 dla częstotliwości
            float freq = (k <= n / 2) ? static_cast<float>(k) : static_cast<float>(n - k);
            float ramp = freq / static_cast<float>(std::max(1, n / 2));

            // Pomnóż każdy element spektrum przez wartość filtru
            cv::Vec2f& bin = spectrum.at<cv::Vec2f>(k, 0);
            bin[0] *= ramp;
            bin[1] *= ramp;
        }

        // Odwrotna transformata Fouriera
        cv::Mat inv;
        cv::idft(spectrum, inv, cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
        
        // Skopiuj przefiltrowaną kolumnę
        inv.rowRange(0, num_emitters).copyTo(filtered.col(a));
    }

    return filtered;
}
