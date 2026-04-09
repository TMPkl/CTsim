#include <opencv2/opencv.hpp>
#include <iostream>
#include "phantom_generator.hpp"

cv::Mat generatePhantom(int size) {
cv::Mat fantom = cv::Mat::zeros(size, size, CV_64F);


 cv::ellipse(fantom, {size/2,size/2}, {90,110}, 0, 0, 360, 1.0, -1);
 cv::ellipse(fantom, {size/2,size/2-28}, {60,70},  0, 0, 360, 0.8, -1);
 cv::ellipse(fantom, {size/2+30,size/2+8}, {20,15},  0, 0, 360, 0.2, -1);

//cv::circle(fantom, {size/2+10,size/2}, 30, 1.0, -1);

    //cv::rectangle(fantom, {size/2-20,size/2-20}, {size/2+20,size/2+20}, 1.0, -1);
return fantom;
}