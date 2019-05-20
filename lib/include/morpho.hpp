#pragma once

#include <opencv2/opencv.hpp>

void dilate(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
            const cv::Mat_<uchar>& kernel);
void erode(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
           const cv::Mat_<uchar>& kernel);
