#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using Segment = std::vector<cv::Point>;

using Segments = std::vector<Segment>;

Segments find_segments(cv::Mat& image);
