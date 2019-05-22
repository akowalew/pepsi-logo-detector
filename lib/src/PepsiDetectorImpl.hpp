#pragma once

#include "PepsiDetector.hpp"

#include "blobs.hpp"

class PepsiDetector::Impl
{
public:
    explicit Impl(const Config& config);

    Logos find_logos(const cv::Mat& bgr) const;

private:
	cv::Mat_<cv::Vec3b> convert_image(const cv::Mat_<cv::Vec3b>& bgr) const;

	Blobs detect_blue_blobs(const cv::Mat_<cv::Vec3b>& hsv) const;

	Blobs detect_red_blobs(const cv::Mat_<cv::Vec3b>& hsv) const;

	cv::Mat_<uchar> extract_blue_color(const cv::Mat_<cv::Vec3b>& hsv) const;

	cv::Mat_<uchar> extract_red_color(const cv::Mat_<cv::Vec3b>& hsv) const;

	cv::Mat_<uchar> threshold_blue_color(const cv::Mat_<cv::Vec3b>& hsv) const;

	cv::Mat_<uchar> threshold_red_color(const cv::Mat_<cv::Vec3b>& hsv) const;

	void filter_color_mask(cv::Mat_<uchar>& mask) const;

	Blobs find_blue_blobs(cv::Mat_<uchar>& blue_color_mask) const;

	Blobs find_red_blobs(cv::Mat_<uchar>& red_color_mask) const;

	void filter_blue_blobs(Blobs& blobs) const;

	void filter_red_blobs(Blobs& blobs) const;

	Blobs::iterator filter_blobs_by_area(Blobs& blobs, BlobAreaRange blob_area_range) const;

	Blobs::iterator filter_blobs_by_hu_moments(Blobs& blobs, HuMomentsArray hu_moments_array,
											   HuMomentRange hu0_range, HuMomentRange hu1_range) const;

	bool blobs_centers_matching(Point red_center, Point blue_center) const;

	Logos match_blobs(const Blobs& red_blobs, const Blobs& blue_blobs) const;

    Config m_config;
};
