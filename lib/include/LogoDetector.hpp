#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using LogoDescription = cv::Rect;

using Logos = std::vector<LogoDescription>;

class LogoDetector
{
public:
	virtual ~LogoDetector() = default;

	virtual Logos find_logos(const cv::Mat& img) const = 0;

	virtual void train() = 0;
};
