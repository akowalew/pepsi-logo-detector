#pragma once

#include <opencv2/opencv.hpp>

void bgr2hsv(const cv::Mat& src, cv::Mat& dst);
void bgr2gray(const cv::Mat& src, cv::Mat& dst);
