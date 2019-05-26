#include "PepsiDetector.hpp"
#include "PepsiDetectorImpl.hpp"

#include <cstdio>

#include <algorithm>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "blobs.hpp"
#include "core.hpp"
#include "drawing.hpp"
#include "format.hpp"
#include "imglog.hpp"
#include "moments.hpp"
#include "utility.hpp"
#include "morpho.hpp"

namespace {

struct BlobAnchors
{
    Point top_left;
    Point bottom_right;
};

using BlobsAnchors = std::vector<BlobAnchors>;

std::ostream& operator<<(std::ostream& os, const HuMoments& hu_moments)
{
    for(const auto hu_moment : hu_moments)
    {
        os << hu_moment << ", ";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const HuMomentsArray& hu_moments_array)
{
    for(const auto& hu_moments : hu_moments_array)
    {
        os << hu_moments << '\n';
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, Point point)
{
    os << '(' << point.x << ", " << point.y << ')';
    return os;
}

std::ostream& operator<<(std::ostream& os, const Points& points)
{
    for(const auto point : points)
    {
        os << point << ' ';
    }

    return os;
}

void log_blobs(const Blobs& blobs, const cv::Vec3b& color, cv::Size img_size, const char* img_name)
{
    if(imglog::enabled())
    {
        cv::Mat_<cv::Vec3b> img = cv::Mat_<cv::Vec3b>::zeros(img_size);
        draw_blobs(blobs, img, color);
        imglog::log(img_name, img);
    }
}

void log_blobs_randomly(const Blobs& blobs, cv::Size img_size, const char* img_name)
{
    if(imglog::enabled())
    {
        cv::Mat_<cv::Vec3b> img = cv::Mat_<cv::Vec3b>::zeros(img_size);
        draw_blobs_randomly(blobs, img);
        imglog::log(img_name, img);
    }
}

BlobAnchors get_blob_anchors(const Blob& blob)
{
    Point top_left = blob.front();
    Point bottom_right = blob.front();
    std::for_each(std::next(blob.begin()), blob.end(),
        [&top_left, &bottom_right](const auto& point)
        {
            if(point.x < top_left.x)
            {
                top_left.x = point.x;
            }

            if(point.y < top_left.y)
            {
                top_left.y = point.y;
            }

            if(point.x > bottom_right.x)
            {
                bottom_right.x = point.x;
            }

            if(point.y > bottom_right.y)
            {
                bottom_right.y = point.y;
            }
        });

    return {top_left, bottom_right};
}

BlobsAnchors get_blobs_anchors(const Blobs& blobs)
{
    spdlog::debug("[PepsiDetector] Getting blobs anchors...");

    auto blobs_anchors = BlobsAnchors();
    blobs_anchors.reserve(blobs.size());

    std::transform(blobs.begin(), blobs.end(),
                   std::back_inserter(blobs_anchors),
                   get_blob_anchors);

    return blobs_anchors;
}

Point get_blob_center(const BlobAnchors& blob_anchors)
{
    const auto center_x = ((blob_anchors.bottom_right.x + blob_anchors.top_left.x) / 2);
    const auto center_y = ((blob_anchors.bottom_right.y + blob_anchors.top_left.y) / 2);
    return {center_x, center_y};
}

Points get_blobs_centers(const BlobsAnchors& blobs_anchors)
{
    spdlog::debug("[PepsiDetector] Getting blobs centers from its anchors...");

    auto blobs_centers = Points();
    blobs_centers.reserve(blobs_anchors.size());

    std::transform(blobs_anchors.begin(), blobs_anchors.end(),
                   std::back_inserter(blobs_centers),
                   get_blob_center);

    return blobs_centers;
}

double calc_points_distance(Point a, Point b) noexcept
{
    const auto dx = (b.x - a.x);
    const auto dy = (b.y - a.y);
    return std::sqrt(dx*dx + dy*dy);
}

} //

// PepsiDetector implementation

PepsiDetector::Impl::Impl(const Config& config)
    :   m_config(config)
{
    spdlog::debug("[PepsiDetector] Initialized");
}

Logos PepsiDetector::Impl::find_logos(const cv::Mat& bgr) const
{
    spdlog::debug("[PepsiDetector] Finding logos on image...");
    imglog::log("Original", bgr);

    const auto enhanced = enhance_image(bgr);
    const auto hsv = convert_image(enhanced);
    const auto blue_blobs = detect_blue_blobs(hsv);
    const auto red_blobs = detect_red_blobs(hsv);
    return match_blobs(red_blobs, blue_blobs);
}

cv::Mat_<cv::Vec3b> PepsiDetector::Impl::enhance_image(const cv::Mat_<cv::Vec3b>& bgr) const
{
    spdlog::debug("Enhancing image...");

    // Unsharp mask kernel 5x5, based on Gaussian blur with amount as 1 and threshold as 0
    float kernel_data[] = {
        -1.0/256,  -4.0/256,   -6.0/256,  -4.0/256, -1.0/256,
        -4.0/256, -16.0/256,  -24.0/256, -16.0/256, -4.0/256,
        -6.0/256, -24.0/256,  476.0/256, -24.0/256, -6.0/256,
        -4.0/256, -16.0/256,  -24.0/256, -16.0/256, -4.0/256,
        -1.0/256,  -4.0/256,   -6.0/256,  -4.0/256, -1.0/256,
    };

    auto kernel = cv::Mat(5, 5, CV_32F, kernel_data);
    auto enhanced = cv::Mat_<cv::Vec3b>{bgr.size()};

    filter_image(bgr, enhanced, kernel);
    imglog::log("Image enhanced", enhanced);

    return enhanced;
}

cv::Mat_<cv::Vec3b> PepsiDetector::Impl::convert_image(const cv::Mat_<cv::Vec3b>& bgr) const
{
    spdlog::debug("[PepsiDetector] Converting image to HSV...");

    auto hsv = cv::Mat_<cv::Vec3b>{bgr.size()};
    bgr2hsv(bgr, hsv);
    return hsv;
}

Blobs PepsiDetector::Impl::detect_blue_blobs(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Detecting blue blobs on image...");

    auto blue_color_mask = extract_blue_color(hsv);
    auto blue_blobs = find_blue_blobs(blue_color_mask);
    filter_blue_blobs(blue_blobs);
    log_blobs(blue_blobs, cv::Vec3b{255, 0, 0}, hsv.size(), "Blue blobs final");

    return blue_blobs;
}

Blobs PepsiDetector::Impl::detect_red_blobs(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Detecting red blobs on image...");

    auto red_color_mask = extract_red_color(hsv);
    auto red_blobs = find_red_blobs(red_color_mask);
    filter_red_blobs(red_blobs);
    log_blobs(red_blobs, cv::Vec3b{0, 0, 255}, hsv.size(), "Red blobs final");

    return red_blobs;
}

cv::Mat_<uchar> PepsiDetector::Impl::extract_blue_color(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Extracting blue color...");

    auto blue_mask = threshold_blue_color(hsv);
    filter_color_mask(blue_mask);
    imglog::log("Blue color mask filtered", blue_mask);

    return blue_mask;
}

cv::Mat_<uchar> PepsiDetector::Impl::extract_red_color(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Extracting red color...");

    auto red_mask = threshold_red_color(hsv);
    filter_color_mask(red_mask);
    imglog::log("Red color mask filtered", red_mask);

    return red_mask;
}

cv::Mat_<uchar> PepsiDetector::Impl::threshold_blue_color(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Thresholding blue color...");

    auto blue_mask = cv::Mat_<uchar>{hsv.size()};
    threshold(hsv, blue_mask, m_config.blue_range);
    imglog::log("Blue color mask", blue_mask);

    return blue_mask;
}

cv::Mat_<uchar> PepsiDetector::Impl::threshold_red_color(const cv::Mat_<cv::Vec3b>& hsv) const
{
    spdlog::debug("[PepsiDetector] Thresholding red color...");

    auto left_red_mask = cv::Mat_<uchar>{hsv.size()};
    auto left_red_range = m_config.red_range;
    left_red_range.min[0] = 0;
    threshold(hsv, left_red_mask, left_red_range);
    imglog::log("Left red color mask", left_red_mask);

    auto right_red_mask = cv::Mat_<uchar>{hsv.size()};
    auto right_red_range = m_config.red_range;
    right_red_range.max[0] = 180;
    threshold(hsv, right_red_mask, right_red_range);
    imglog::log("Right red color mask", right_red_mask);

    auto red_mask = cv::Mat_<uchar>{hsv.size()};
    bitwise_or(left_red_mask, right_red_mask, red_mask);
    imglog::log("Red color mask", red_mask);

    return red_mask;
}

Blobs PepsiDetector::Impl::find_blue_blobs(cv::Mat_<uchar>& blue_color_mask) const
{
    spdlog::debug("[PepsiDetector] Finding blue blobs...");

    auto blue_blobs = find_blobs(blue_color_mask);
    log_blobs_randomly(blue_blobs, blue_color_mask.size(), "Blue blobs");

    return blue_blobs;
}

Blobs PepsiDetector::Impl::find_red_blobs(cv::Mat_<uchar>& red_color_mask) const
{
    spdlog::debug("[PepsiDetector] Finding red blobs...");

    auto red_blobs = find_blobs(red_color_mask);
    log_blobs_randomly(red_blobs, red_color_mask.size(), "Red blobs");

    return red_blobs;
}

void PepsiDetector::Impl::filter_red_blobs(Blobs& blobs) const
{
    spdlog::debug("[PepsiDetector] Filtering red blobs...");

    filter_blobs_by_area(blobs, m_config.red_blob_area_range);
    const auto blobs_hu_moments = calc_blobs_hu_moments(blobs);
    spdlog::debug("[PepsiDetector] Red blobs after by area filtering:\n{}", blobs_hu_moments);

    filter_blobs_by_hu_moments(blobs, blobs_hu_moments,
                               m_config.red_blob_hu0_range,
                               m_config.red_blob_hu1_range);
    spdlog::debug("[PepsiDetector] Red blobs after by hu filtering:\n{}", calc_blobs_hu_moments(blobs));
}

void PepsiDetector::Impl::filter_blue_blobs(Blobs& blobs) const
{
    spdlog::debug("[PepsiDetector] Filtering blue blobs...");

    filter_blobs_by_area(blobs, m_config.blue_blob_area_range);
    auto blobs_hu_moments = calc_blobs_hu_moments(blobs);
    spdlog::debug("[PepsiDetector] Blue blobs after by area filtering:\n{}", blobs_hu_moments);

    filter_blobs_by_hu_moments(blobs, blobs_hu_moments,
                               m_config.blue_blob_hu0_range,
                               m_config.blue_blob_hu1_range);
    spdlog::debug("[PepsiDetector] Blue blobs after by hu filtering:\n{}", calc_blobs_hu_moments(blobs));
}

bool PepsiDetector::Impl::blobs_centers_matching(Point red_center, Point blue_center) const
{
    if(/*red_center.x < blue_center.x
        && red_center.y < blue_center.y*/
        /*&&*/ calc_points_distance(red_center, blue_center) <= m_config.max_blobs_centers_distance)
    {
        return true;
    }

    return false;
}

Logos PepsiDetector::Impl::match_blobs(const Blobs& red_blobs, const Blobs& blue_blobs) const
{
    spdlog::debug("[PepsiDetector] Matching blobs...");

    const auto red_blobs_anchors = get_blobs_anchors(red_blobs);
    const auto red_blobs_centers = get_blobs_centers(red_blobs_anchors);

    const auto blue_blobs_anchors = get_blobs_anchors(blue_blobs);
    const auto blue_blobs_centers = get_blobs_centers(blue_blobs_anchors);

    spdlog::debug("[PepsiDetector] Red centers: {}", red_blobs_centers);
    spdlog::debug("[PepsiDetector] Blue centers: {}", blue_blobs_centers);

    Logos logos;
    const auto logos_max = std::min(red_blobs.size(), blue_blobs.size());
    logos.reserve(logos_max);

    const auto red_blobs_size = red_blobs.size();
    const auto blue_blobs_size = blue_blobs.size();
    for(auto red_idx = 0; red_idx < red_blobs_size; ++red_idx)
    {
        auto red_center = red_blobs_centers[red_idx];
        for(auto blue_idx = 0; blue_idx < blue_blobs_size; ++blue_idx)
        {
            auto blue_center = blue_blobs_centers[blue_idx];
            if(blobs_centers_matching(red_center, blue_center))
            {
                const auto red_anchors = red_blobs_anchors[red_idx];
                const auto blue_anchors = blue_blobs_anchors[blue_idx];

                const auto top_left_x = std::min(red_anchors.top_left.x, blue_anchors.top_left.x);
                const auto top_left_y = std::min(red_anchors.top_left.y, blue_anchors.top_left.y);

                const auto bottom_right_x = std::max(red_anchors.bottom_right.x, blue_anchors.bottom_right.x);
                const auto bottom_right_y = std::max(red_anchors.bottom_right.y, blue_anchors.bottom_right.y);

                logos.emplace_back(Point{top_left_x, top_left_y}, Point{bottom_right_x, bottom_right_y});
            }
        }
    }

    logos.shrink_to_fit();
    return logos;
}

void PepsiDetector::Impl::filter_color_mask(cv::Mat_<uchar>& color_mask) const
{
    spdlog::debug("[PepsiDetector] Filtering color mask...");

    const cv::Mat_<uchar> kernel = cv::Mat_<uchar>::ones(cv::Size{3, 3});
    auto tmp = cv::Mat_<uchar>(color_mask.size());

    erode(color_mask, tmp, kernel);
    dilate(tmp, color_mask, kernel);
}

Blobs::iterator PepsiDetector::Impl::filter_blobs_by_area(Blobs& blobs, BlobAreaRange blob_area_range) const
{
    spdlog::debug("[PepsiDetector] Filtering blobs by area...");

    auto is_wrong_area =
        [&blob_area_range](const auto& blob)
        {
            const auto area = blob.size();
            return (area < blob_area_range.min || area > blob_area_range.max);
        };

    return blobs.erase(std::remove_if(blobs.begin(), blobs.end(), is_wrong_area), blobs.end());
}

Blobs::iterator PepsiDetector::Impl::filter_blobs_by_hu_moments(Blobs& blobs, HuMomentsArray hu_moments_array,
                                                                HuMomentRange hu0_range, HuMomentRange hu1_range) const
{
    spdlog::debug("[PepsiDetector] Filtering blobs by hu_moments...");

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
