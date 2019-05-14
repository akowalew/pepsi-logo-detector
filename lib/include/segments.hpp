#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using SegmentsAnchors = std::vector<cv::Point>;
using SegmentsMats = std::vector<cv::Mat>;

using Segments = std::pair<SegmentsAnchors, SegmentsMats>;

Segments find_segments(cv::Mat& image);
