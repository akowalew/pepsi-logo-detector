#pragma once

#include "PepsiDetector.hpp"

#include "blobs.hpp"

class PepsiDetector::Impl
{
public:
    explicit Impl(const Config& config);

    Logos find_logos(const cv::Mat& img) const;

    void train();

    const Config& config() const noexcept;

private:
	cv::Mat convert_image(const cv::Mat& img) const;

	Blobs extract_blue_blobs(const cv::Mat& hsv) const;

	Blobs extract_red_blobs(const cv::Mat& hsv) const;

	Blobs find_blue_blobs(const cv::Mat& hsv) const;

	Blobs find_red_blobs(const cv::Mat& hsv) const;

	bool blobs_centers_matching(Point red_center, Point blue_center) const;

	Logos match_blobs(const Blobs& red_blobs, const Blobs& blue_blobs) const;

	cv::Mat_<uchar> extract_color(const cv::Mat& hsv, const ColorRange& range) const;

	cv::Mat_<uchar> extract_colors(const cv::Mat& hsv, const ColorRanges& ranges) const;

	void filter_color_mask(cv::Mat_<uchar>& mask) const;

	Blobs::iterator filter_blobs_by_area(Blobs& blobs, BlobAreaRange blob_area_range) const;

	Blobs::iterator filter_blobs_by_hu_moments(Blobs& blobs, HuMomentsArray hu_moments_array,
											   HuMomentRange hu0_range, HuMomentRange hu1_range) const;

    Config m_config;
};
