#pragma once

#include <opencv2/opencv.hpp>

#include "blobs.hpp"

void draw_blobs(const Blobs& blobs, cv::Mat_<cv::Vec3b>& img, const cv::Vec3b& color);

void draw_blobs_randomly(const Blobs& blobs, cv::Mat_<cv::Vec3b>& img);

void draw_rectangle_centered(cv::Mat_<uchar>& img, cv::Size size, uchar color, int thickness = 1);
