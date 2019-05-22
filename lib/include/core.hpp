#pragma once

#include <opencv2/opencv.hpp>

#include "types.hpp"
#include "utility.hpp"

void threshold(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<uchar>& dst,
		       const ColorRange& color_range);

void bitwise_or(const cv::Mat_<uchar>& src1, const cv::Mat_<uchar>& src2, cv::Mat_<uchar>& dst);

void filter_image(const cv::Mat3b& src, cv::Mat3b& dst, const cv::Mat1f& kernel);

bool images_equal(const cv::Mat& img1, const cv::Mat& img2);
