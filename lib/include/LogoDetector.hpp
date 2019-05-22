#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using Logo = cv::Rect;

using Logos = std::vector<Logo>;

class LogoDetector
{
public:
	virtual ~LogoDetector() = default;

	virtual Logos find_logos(const cv::Mat& img) const = 0;
};
