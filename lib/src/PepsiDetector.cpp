#include "PepsiDetector.hpp"
#include "PepsiDetectorImpl.hpp"

#include <cstdio>

#include <algorithm>
#include <chrono>

#include "blobs.hpp"
#include "core.hpp"
#include "format.hpp"
#include "moments.hpp"
#include "utility.hpp"

namespace {

void print_moments(const HuMomentsArray& hu_moments)
{
    for(const auto& hu : hu_moments)
    {
        for(const auto moment : hu)
        {
            printf("%8.8lf ", moment);
        }

        printf("\n");
    }
}

void display_blobs(cv::Size mat_size, const Blobs& blobs, const char* window_name)
{
    cv::Mat mat = cv::Mat::zeros(mat_size, CV_8UC3);
    for(const auto& blob: blobs)
    {
        const auto color = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
        for(const auto point : blob)
        {
            mat.at<cv::Vec3b>(point) = color;
        }
    }

    cv::imshow(window_name, mat);
    cv::moveWindow(window_name, 0, 0);
}

} //

/**
 * @brief Default constructor for PepsiDetector's Config. Provides default values for params
 */
PepsiDetector::Config::Config()
    :   blue_range{{{100, 75, 0}}, {{130, 255, 255}}}
    ,   blue_blob_area_range{200, 1000000}
    ,   blue_blob_hu0_range{0.30, 0.41}
    ,   blue_blob_hu1_range{0.05, 0.10}

    ,   red_ranges{{
            {{{0, 75, 75}}, {{10, 255, 255}}},
            {{{165, 75, 75}}, {{180, 255, 255}}}
        }}
    ,   red_blob_area_range{200, 10000}
    ,	red_blob_hu0_range{0.18, 0.20}
	,	red_blob_hu1_range{0.006, 0.010}
{}

// PepsiDetector implementation

PepsiDetector::Impl::Impl(const Config& config)
    :   m_config(config)
{}

Logos PepsiDetector::Impl::find_logos(const cv::Mat& img) const
{
    cv::imshow("Original", img);
    cv::moveWindow("Original", 0, 0);

    const auto hsv = convert_image(img);
    const auto blue_blobs = extract_blue_blobs(hsv);
    const auto red_blobs = extract_red_blobs(hsv);
    // return match_blobs(red_blobs, blue_blobs);
    return {};
}

void PepsiDetector::Impl::train() {}

const PepsiDetector::Config& PepsiDetector::Impl::config() const noexcept
{
    return m_config;
}

cv::Mat PepsiDetector::Impl::convert_image(const cv::Mat& img) const
{
    auto hsv = cv::Mat{img.size(), CV_8UC3};
    bgr2hsv(img, hsv);
    return hsv;
}

Blobs PepsiDetector::Impl::extract_blue_blobs(const cv::Mat& hsv) const
{
    auto blobs = find_blue_blobs(hsv);

    filter_blobs_by_area(blobs, m_config.blue_blob_area_range);

    auto blobs_hu_moments = calc_blobs_hu_moments(blobs);
    printf("After by area filtering:\n");
    print_moments(blobs_hu_moments);

    filter_blobs_by_hu_moments(blobs, blobs_hu_moments,
                               m_config.blue_blob_hu0_range,
                               m_config.blue_blob_hu1_range);

    blobs_hu_moments = calc_blobs_hu_moments(blobs);
    printf("After by hu filtering:\n");
    print_moments(blobs_hu_moments);

    display_blobs(hsv.size(), blobs, "Blue blobs final");
    return blobs;
}

Blobs PepsiDetector::Impl::extract_red_blobs(const cv::Mat& hsv) const
{
    auto blobs = find_red_blobs(hsv);

    filter_blobs_by_area(blobs, m_config.red_blob_area_range);

    auto blobs_hu_moments = calc_blobs_hu_moments(blobs);
    printf("After by area filtering:\n");
    print_moments(blobs_hu_moments);

    filter_blobs_by_hu_moments(blobs, blobs_hu_moments,
                               m_config.red_blob_hu0_range,
                               m_config.red_blob_hu1_range);

    blobs_hu_moments = calc_blobs_hu_moments(blobs);
    printf("After by hu filtering:\n");
    print_moments(blobs_hu_moments);

    display_blobs(hsv.size(), blobs, "Red blobs final");
    return blobs;
}

Blobs PepsiDetector::Impl::find_red_blobs(const cv::Mat& hsv) const
{
    auto color_mask = extract_colors(hsv, { m_config.red_ranges.begin(), m_config.red_ranges.end() });
    filter_color_mask(color_mask);

    auto red_blobs = find_blobs(color_mask);

    return red_blobs;
}

Blobs PepsiDetector::Impl::find_blue_blobs(const cv::Mat& hsv) const
{
    auto blue_color_mask = extract_color(hsv, m_config.blue_range);
    filter_color_mask(blue_color_mask);

    auto blue_blobs = find_blobs(blue_color_mask);

    return blue_blobs;
}

Logos PepsiDetector::Impl::match_blobs(const Blobs& red_blobs, const Blobs& blue_blobs) const
{
    return {};
}

cv::Mat_<uchar> PepsiDetector::Impl::extract_color(const cv::Mat& hsv, const ColorRange& range) const
{
    auto mask = cv::Mat_<uchar>{hsv.size()};
    double_threshold(hsv, mask, adapt_array(range.min), adapt_array(range.max));

    cv::imshow("Color mask", mask);
    cv::moveWindow("Color mask", 0, 0);

    return mask;
}

cv::Mat_<uchar> PepsiDetector::Impl::extract_colors(const cv::Mat& hsv, const ColorRanges& ranges) const
{
    auto color_mask = cv::Mat_<uchar>{hsv.size()};
    auto result_mask = cv::Mat_<uchar>{hsv.size()};
    for(const auto& range : ranges)
    {
        double_threshold(hsv, color_mask, adapt_array(range.min), adapt_array(range.max));
        bitwise_or(color_mask, result_mask, result_mask);
    }

    cv::imshow("Color mask", result_mask);
    cv::moveWindow("Color mask", 0, 0);

    return result_mask;
}

void PepsiDetector::Impl::filter_color_mask(cv::Mat_<uchar>& color_mask) const
{
    const cv::Mat_<uchar> kernel = cv::Mat_<uchar>::ones(cv::Size{3, 3});
    cv::erode(color_mask, color_mask, kernel);
    cv::dilate(color_mask, color_mask, kernel);

    cv::imshow("Color mask filtered", color_mask);
    cv::moveWindow("Color mask filtered", 0, 0);
}

Blobs::iterator PepsiDetector::Impl::filter_blobs_by_area(Blobs& blobs, BlobAreaRange blob_area_range) const
{
    auto is_wrong_area =
        [&blob_area_range](const auto& blob)
        {
            const auto area = blob.size();
            return (area < blob_area_range.min || area > blob_area_range.max);
        };

    return blobs.erase(std::remove_if(blobs.begin(), blobs.end(), is_wrong_area),
                       blobs.end());
}

Blobs::iterator PepsiDetector::Impl::filter_blobs_by_hu_moments(Blobs& blobs, HuMomentsArray hu_moments_array,
                                                                HuMomentRange hu0_range, HuMomentRange hu1_range) const
{
    auto hu_it = hu_moments_array.begin();
    const auto hu_not_match =
        [hu0_range, hu1_range, &hu_it](const auto& blob)
        {
            const auto& hu = *(hu_it++);

            return ((hu[0] < hu0_range.min || hu[0] > hu0_range.max)
                || ((hu[1] < hu1_range.min) || hu[1] > hu1_range.max));
        };

    return blobs.erase(std::remove_if(blobs.begin(), blobs.end(), hu_not_match),
                       blobs.end());
}

// PepsiDetector public methods

PepsiDetector::PepsiDetector(const Config& config)
	:	m_impl(std::make_unique<Impl>(config))
{}

PepsiDetector::~PepsiDetector() = default;

PepsiDetector::PepsiDetector(PepsiDetector&& other) = default;
PepsiDetector& PepsiDetector::operator=(PepsiDetector&& other) = default;

PepsiDetector::PepsiDetector(const PepsiDetector& other)
{
    *this = other;
}

PepsiDetector& PepsiDetector::operator=(const PepsiDetector &other)
{
    if(&other != this)
    {
        m_impl = std::make_unique<Impl>(*other.m_impl);
    }

    return *this;
}

Logos PepsiDetector::find_logos(const cv::Mat& img) const
{
    return m_impl->find_logos(img);
}

void PepsiDetector::train()
{
    m_impl->train();
}

const PepsiDetector::Config& PepsiDetector::config() const noexcept
{
    return m_impl->config();
}
