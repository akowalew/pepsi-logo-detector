#pragma once

#include <opencv2/opencv.hpp>

void threshold(const cv::Mat& src, cv::Mat& dst,
			   double thresh);
void double_threshold(const cv::Mat& src, cv::Mat_<uchar>& dst,
					  cv::InputArray lower_bound, cv::InputArray upper_bound);

void bitwise_or(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
