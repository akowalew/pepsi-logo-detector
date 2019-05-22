#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using Point = cv::Point;
using Points = std::vector<cv::Point>;
using PointsSet = std::vector<Points>;

using Rect = cv::Rect;
using Rects = std::vector<cv::Rect>;

template<typename T>
struct ValueRange
{
	T min;
	T max;
};

using ColorRange = ValueRange<cv::Vec3b>;
using ColorRanges = std::vector<ColorRange>;
