#pragma once

#include <opencv2/opencv.hpp>

#include "utility.hpp"

void threshold(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<uchar>& dst,
		       cv::Vec3b min, cv::Vec3b max);

void bitwise_or(const cv::Mat_<uchar>& src1, const cv::Mat_<uchar>& src2, cv::Mat_<uchar>& dst);

bool images_equal(const cv::Mat& img1, const cv::Mat& img2);
