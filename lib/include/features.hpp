#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

struct Moments {};

Moments calc_moments(const cv::Mat& src);
double calc_area(const cv::Mat& src);
double calc_perimeter(const cv::Mat& src);
