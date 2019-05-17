#include "PepsiDetector.hpp"

#include <cstdio>

#include <algorithm>
#include <chrono>

#include "blobs.hpp"
#include "core.hpp"
#include "format.hpp"
#include "moments.hpp"

template<typename T, std::size_t N>
cv::_InputArray adapt_array(const std::array<T, N>& array)
{
	return cv::_InputArray{array.data(), static_cast<int>(array.size())};
}

/**
 * @brief Default constructor for PepsiDetector's Config. Provides default values for params
 */
PepsiDetector::Config::Config()
    :   min_blue_hsv({{110, 100, 20}})
    ,   max_blue_hsv({{130, 255, 255}})
    ,   min_left_red_hsv({{0, 150, 150}})
    ,   max_left_red_hsv({{15, 255, 255}})
    ,   min_right_red_hsv({{165, 150, 150}})
    ,   max_right_red_hsv({{180, 255, 255}})
    ,   min_area(1000.0)
    ,   max_area(10000.0)
    ,   min_malinowska(0.6)
    ,   max_malinowska(0.9)
    ,	min_hu0(0.18)
    ,	max_hu0(0.25)
	,	min_hu1(0.006)
    ,	max_hu1(0.020)
	// ,	min_hu5(-0.008)
    // ,	max_hu5(-0.004)
{}

PepsiDetector::PepsiDetector(const Config& config)
	:	_config(config)
{

}

Logos PepsiDetector::find_logos(const cv::Mat& src)
{
	// Preconditions
	CV_Assert(src.channels() == 3);

	cv::imshow("Original", src);
	cv::moveWindow("Original", 0, 0);

	// Convert image
	auto hsv = cv::Mat{src.size(), CV_8UC3};
	bgr2hsv(src, hsv);

	//const auto blue_blobs = match_blue_parts(hsv);
	const auto red_blobs = match_red_parts(hsv);

	cv::Mat tmp = cv::Mat::zeros(src.size(), CV_8UC3);
	for(const auto& blob: red_blobs)
	{
		const auto color = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
		for(const auto point : blob)
		{
			tmp.at<cv::Vec3b>(point) = color;
		}
	}

	cv::imshow("Red blobs final", tmp);
	cv::moveWindow("Red blobs final", 0, 0);

	return {};
}

Blobs PepsiDetector::match_blue_parts(const cv::Mat& hsv)
{
	auto blue_mask = cv::Mat_<uchar>{hsv.size()};
	double_threshold(hsv, blue_mask,
					 adapt_array(_config.min_blue_hsv),
					 adapt_array(_config.max_blue_hsv));
	cv::imshow("Blue mask", blue_mask);

	const auto blue_kernel = getStructuringElement(cv::MORPH_RECT, cv::Size{3, 3});
	cv::erode(blue_mask, blue_mask, blue_kernel);
	cv::dilate(blue_mask, blue_mask, blue_kernel);
	cv::imshow("Blue mask filtered", blue_mask);

	const auto start = std::chrono::system_clock::now();
	const auto blue_blobs = find_blobs(blue_mask);
	const auto stop = std::chrono::system_clock::now();
	const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	printf("Took %ld milliseconds\n", diff.count());
	printf("Find %lu blue blobs\n", blue_blobs.size());

	for(const auto& blue_blob : blue_blobs)
	{
		const auto hu_moments = calc_hu_moments(blue_blob);
		for(const auto hu : hu_moments)
		{
			printf("%10.10lf ", hu);
		}
	}

	printf("\n");

	return blue_blobs;
}

Blobs PepsiDetector::match_red_parts(const cv::Mat& hsv)
{
	// Detect red parts
	auto left_red_mask = cv::Mat_<uchar>{hsv.size()};
	double_threshold(hsv, left_red_mask,
					 adapt_array(_config.min_left_red_hsv),
					 adapt_array(_config.max_left_red_hsv));

	auto right_red_mask = cv::Mat_<uchar>{hsv.size()};
	double_threshold(hsv, right_red_mask,
					 adapt_array(_config.min_right_red_hsv),
					 adapt_array(_config.max_right_red_hsv));

	// Join red masks
	auto red_mask = cv::Mat_<uchar>{hsv.size()};
	bitwise_or(left_red_mask, right_red_mask, red_mask);
	cv::imshow("Red mask", red_mask);
	cv::moveWindow("Red mask", 0, 0);

	// Filter mask against noise
	const auto red_kernel = getStructuringElement(cv::MORPH_RECT, cv::Size{3, 3});
	cv::erode(red_mask, red_mask, red_kernel);
	cv::dilate(red_mask, red_mask, red_kernel);
	cv::dilate(red_mask, red_mask, red_kernel);
	cv::imshow("Red mask filtered", red_mask);
	cv::moveWindow("Red mask filtered", 0, 0);

	// Find blobs in mask
	const auto start = std::chrono::system_clock::now();
	auto red_blobs = find_blobs(red_mask);
	const auto stop = std::chrono::system_clock::now();
	const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	printf("Find %lu red blobs\n", red_blobs.size());

	// Filter out blobs with wrong area
	auto is_wrong_area =
		[this](const auto& blob)
		{
			const auto area = blob.size();
			return (area < _config.min_area || area > _config.max_area);
		};

	red_blobs.erase(std::remove_if(red_blobs.begin(), red_blobs.end(), is_wrong_area),
					red_blobs.end());

	// Calculate hu moments for each blob
	auto hu_moments = std::vector<HuMoments>(red_blobs.size());
	std::transform(red_blobs.begin(), red_blobs.end(), hu_moments.begin(),
				   [](const auto& blob) { return calc_hu_moments(blob); });
	for(const auto& hu : hu_moments)
	{
		for(const auto moment : hu)
		{
			printf("% 8.8lf ", moment);
		}

		printf("\n");
	}

	// Filter out blobs which doesn't match needed hu moments
	auto hu_it = hu_moments.begin();
	const auto hu_not_match =
		[this, &hu_it](const auto& blob)
		{
			const auto& hu = *(hu_it++);

			return ((hu[0] < _config.min_hu0 || hu[1] > _config.max_hu0)
				|| ((hu[1] < _config.min_hu1) || hu[1] > _config.max_hu1));
				// || ((hu[5] < _config.min_hu5) || hu[5] > _config.max_hu5));
		};

	red_blobs.erase(std::remove_if(red_blobs.begin(), red_blobs.end(), hu_not_match),
					red_blobs.end());

	return red_blobs;
}
