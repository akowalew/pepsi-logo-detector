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

		std::array<uchar, 3> min_blue_hsv;
		std::array<uchar, 3> max_blue_hsv;
		std::array<uchar, 3> min_left_red_hsv;
		std::array<uchar, 3> max_left_red_hsv;
		std::array<uchar, 3> min_right_red_hsv;
		std::array<uchar, 3> max_right_red_hsv;
		int min_area;
		int max_area;
		double min_malinowska;
		double max_malinowska;
		double min_hu0;
		double max_hu0;
		double min_hu1;
		double max_hu1;
		// double min_hu5;
		// double max_hu5;
	};

	explicit PepsiDetector(const Config& config = Config());

	Logos find_logos(const cv::Mat& img);

	const Config& config() const noexcept;

private:
	Blobs match_blue_parts(const cv::Mat& img);

	Blobs match_red_parts(const cv::Mat& img);

	Config _config;
};
