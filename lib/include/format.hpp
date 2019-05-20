#pragma once

#include <opencv2/opencv.hpp>

void bgr2hsv(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<cv::Vec3b>& dst);
