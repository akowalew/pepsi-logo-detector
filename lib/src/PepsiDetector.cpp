#include "PepsiDetector.hpp"

#include "core.hpp"
#include "format.hpp"
#include "segments.hpp"

template<typename T, std::size_t N>
cv::_InputArray adapt_array(const std::array<T, N>& array)
{
	return cv::_InputArray{array.data(), static_cast<int>(array.size())};
}

/**
 * @brief Default constructor for PepsiDetector's Config. Provides default values for params
 */
PepsiDetector::Config::Config()
    :   blue_hsv_min({{110, 100, 20}})
    ,   blue_hsv_max({{130, 255, 255}})
    ,   left_red_hsv_min({{0, 150, 150}})
    ,   left_red_hsv_max({{15, 255, 255}})
    ,   right_red_hsv_min({{165, 150, 150}})
    ,   right_red_hsv_max({{180, 255, 255}})
    ,   area_min(10.0)
    ,   area_max(2000.0)
    ,   malinowska_min(0.6)
    ,   malinowska_max(0.9)
{}

PepsiDetector::PepsiDetector(const Config& config)
	:	_config(config)
{

}

Logos PepsiDetector::find_logos(const cv::Mat& src)
{
	// Preconditions
	// CV_Assert(src.depth() == sizeof(uchar));
	CV_Assert(src.channels() == 3);

	// Convert image
	auto hsv = cv::Mat{src.size(), CV_8UC3};
	bgr2hsv(src, hsv);

	// Match blue part
	auto blue_mask = cv::Mat{hsv.size(), CV_8UC1};
	double_threshold(hsv, blue_mask,
					 adapt_array(_config.blue_hsv_min),
					 adapt_array(_config.blue_hsv_max));

	// Find blue segments
	auto blue_segments = find_segments(blue_mask);


	// Match red part
	auto left_red_mask = cv::Mat{hsv.size(), CV_8UC1};
	double_threshold(hsv, left_red_mask,
					 adapt_array(_config.left_red_hsv_min),
					 adapt_array(_config.left_red_hsv_max));

	auto right_red_mask = cv::Mat{hsv.size(), CV_8UC1};
	double_threshold(hsv, right_red_mask,
					 adapt_array(_config.right_red_hsv_min),
					 adapt_array(_config.right_red_hsv_max));

	// Join red masks
	auto red_mask = cv::Mat{hsv.size(), CV_8UC1};
	bitwise_or(left_red_mask, right_red_mask, red_mask);

	return {};
}
