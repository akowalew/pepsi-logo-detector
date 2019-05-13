#pragma once

#include <opencv2/opencv.hpp>

void dilate(const cv::Mat& src, cv::Mat& dst,
            cv::InputArray kernel);
void erode(const cv::Mat& src, cv::Mat& dst,
           cv::InputArray kernel);
