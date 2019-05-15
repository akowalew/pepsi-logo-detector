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
    :   blue_hsv_min({{110, 100, 20}})
    ,   blue_hsv_max({{130, 255, 255}})
    ,   left_red_hsv_min({{0, 150, 150}})
    ,   left_red_hsv_max({{15, 255, 255}})
    ,   right_red_hsv_min({{165, 150, 150}})
    ,   right_red_hsv_max({{180, 255, 255}})
    ,   min_area(1000.0)
    ,   max_area(2000.0)
    ,   min_malinowska(0.6)
    ,   max_malinowska(0.9)
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

	return {};
}

Blobs PepsiDetector::match_blue_parts(const cv::Mat& hsv)
{
	auto blue_mask = cv::Mat_<uchar>{hsv.size()};
	double_threshold(hsv, blue_mask,
					 adapt_array(_config.blue_hsv_min),
					 adapt_array(_config.blue_hsv_max));
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
					 adapt_array(_config.left_red_hsv_min),
					 adapt_array(_config.left_red_hsv_max));

	auto right_red_mask = cv::Mat_<uchar>{hsv.size()};
	double_threshold(hsv, right_red_mask,
					 adapt_array(_config.right_red_hsv_min),
					 adapt_array(_config.right_red_hsv_max));

	// Join red masks
	auto red_mask = cv::Mat_<uchar>{hsv.size()};
	bitwise_or(left_red_mask, right_red_mask, red_mask);
	cv::imshow("Red mask", red_mask);

	// Filter mask against noise
	const auto red_kernel = getStructuringElement(cv::MORPH_RECT, cv::Size{3, 3});
	cv::erode(red_mask, red_mask, red_kernel);
	cv::dilate(red_mask, red_mask, red_kernel);
	cv::dilate(red_mask, red_mask, red_kernel);
	cv::imshow("Red mask filtered", red_mask);

	// Find blobs in mask
	const auto start = std::chrono::system_clock::now();
	auto red_blobs = find_blobs(red_mask);
	const auto stop = std::chrono::system_clock::now();
	const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	printf("Took %ld milliseconds\n", diff.count());
	printf("Find %lu red blobs\n", red_blobs.size());

	// Filter out too small blobs
	const auto is_blob_too_small =
		[min_area = _config.min_area](const auto& blob)
		{
			return (blob.size() < min_area);
		};

	red_blobs.erase(std::remove_if(red_blobs.begin(), red_blobs.end(), is_blob_too_small),
					red_blobs.end());

	// Filter out too big blobs
	const auto is_blob_too_big =
		[max_area = _config.max_area](const auto& blob)
		{
			return (blob.size() > max_area);
		};

	red_blobs.erase(std::remove_if(red_blobs.begin(), red_blobs.end(), is_blob_too_big),
					red_blobs.end());

	// Print moments
	for(const auto& red_blob : red_blobs)
	{
		const auto hu_moments = calc_hu_moments(red_blob);
		for(const auto hu : hu_moments)
		{
			printf("% 8.8lf ", hu);
		}

		printf("\n");
	}

	return red_blobs;
}
