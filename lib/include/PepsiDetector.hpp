#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

#include "blobs.hpp"

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
		int min_area;
		int max_area;
		double min_malinowska;
		double max_malinowska;
	};

	explicit PepsiDetector(const Config& config = Config());

	Logos find_logos(const cv::Mat& img);

	const Config& config() const noexcept;

private:
	Blobs match_blue_parts(const cv::Mat& img);

	Blobs match_red_parts(const cv::Mat& img);

	Config _config;
};
