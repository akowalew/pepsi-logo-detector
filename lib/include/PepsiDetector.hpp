#pragma once

#include <array>
#include <vector>
#include <filesystem>

#include <opencv2/opencv.hpp>

using LogoDescription = cv::Rect;

using Logos = std::vector<LogoDescription>;

class PepsiDetector
{
public:
	struct Config
	{
        Config();

		std::array<uchar, 3> blue_hsv_min;
		std::array<uchar, 3> blue_hsv_max;
		std::array<uchar, 3> left_red_hsv_min;
		std::array<uchar, 3> left_red_hsv_max;
		std::array<uchar, 3> right_red_hsv_min;
		std::array<uchar, 3> right_red_hsv_max;
		double area_min;
		double area_max;
		double malinowska_min;
		double malinowska_max;
	};

	explicit PepsiDetector(const Config& config = Config());

	Logos find_logos(const cv::Mat& image);

	const Config& config() const noexcept;

private:
	Config _config;
};
