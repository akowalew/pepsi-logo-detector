#include "segments.hpp"

#include <cassert>

#include <algorithm>
#include <optional>
#include <stack>
#include <vector>

using Blob = std::vector<cv::Point>;
using Blobs = std::vector<Blob>;

struct Shift
{
    std::int8_t sx;
    std::int8_t sy;
};

static constexpr std::array<Shift, 8> Shifts = {
    Shift{ 1,   0},
    Shift{ 1,   1},
    Shift{ 0,   1},
    Shift{-1,   1},
    Shift{-1,   0},
    Shift{-1,  -1},
    Shift{ 0,  -1},
    Shift{ 1,  -1},
};

bool is_point_valid(const cv::Mat& img, cv::Point point)
{
    return (point.x >= 0
         && point.y >= 0
         && point.y < img.rows
         && point.x < img.cols);
}

bool is_point_set(const cv::Mat& img, cv::Point point)
{
    return (img.at<uchar>(point) != 0);
}

void clear_point(cv::Mat& img, cv::Point point)
{
    img.at<uchar>(point) = 0;
}

void set_point(cv::Mat& img, cv::Point point)
{
    img.at<uchar>(point) = 255;
}

std::optional<cv::Point> find_next_point_from(const cv::Mat& img, cv::Point point)
{
    CV_Assert(img.depth() == CV_8U);
    CV_Assert(img.channels() == 1);
    CV_Assert(is_point_valid(img, point));

    for(const auto shift : Shifts)
    {
        const auto next_point = cv::Point(point.x + shift.sx, point.y + shift.sy);
        if(is_point_valid(img, next_point) && is_point_set(img, next_point))
        {
            return next_point;
        }
    }

    return {};
}

std::optional<cv::Point> find_next_point_in_blob(const cv::Mat& img, const Blob& blob)
{
    CV_Assert(img.depth() == CV_8U);
    CV_Assert(img.channels() == 1);

    for(const auto& point : blob)
    {
        const auto next_point = find_next_point_from(img, point);
        if(next_point)
        {
            return next_point;
        }
    }

    return {};
}

Blob find_blob(cv::Mat& img, cv::Point first)
{
    CV_Assert(img.depth() == CV_8U);
    CV_Assert(img.channels() == 1);
    CV_Assert(is_point_valid(img, first));
    CV_Assert(is_point_set(img, first));

    auto point = first;
    auto blob = Blob();
    blob.reserve(1024);

    do
    {
        clear_point(img, point);
        blob.emplace_back(point);

        if(const auto next_point = find_next_point_from(img, point); next_point)
        {
            point = *next_point;
            continue;
        }

        if(const auto next_point = find_next_point_in_blob(img, blob); next_point)
        {
            point = *next_point;
            continue;
        }
    }
    while(false);

    blob.shrink_to_fit();
    return blob;
}

Blobs find_blobs(cv::Mat& img)
{
    CV_Assert(img.depth() == CV_8U);
	CV_Assert(img.channels() == 1);
    CV_Assert(img.isContinuous());

	auto blobs = Blobs();
    blobs.reserve(1024); // Typically it should be less than this value

    auto ptr = reinterpret_cast<uchar*>(img.data);
    auto point = cv::Point();
    for(point.y = 0; point.y < img.rows; ++point.y)
    {
        for(point.x = 0; point.x < img.cols; ++point.x)
        {
            if(*ptr)
            {
                blobs.push_back(find_blob(img, point));
            }

            ptr++;
        }
    }

    blobs.shrink_to_fit();
    return blobs;
}

cv::Point get_top_left(const Blob& blob)
{
    auto top_left = blob.front();
    std::for_each(std::next(blob.begin()), blob.end(),
        [&top_left](auto point)
        {
            if(point.x < top_left.x)
            {
                top_left.x = point.x;
            }

            if(point.y < top_left.y)
            {
                top_left.y = point.y;
            }
        });

    return top_left;
}

std::vector<cv::Point> get_top_lefts(const Blobs& blobs)
{
    auto top_lefts = std::vector<cv::Point>();
    top_lefts.reserve(blobs.size());

    std::transform(blobs.begin(), blobs.end(),
                   std::back_inserter(top_lefts),
                   get_top_left);

    return top_lefts;
}

cv::Point get_bottom_right(const Blob& blob)
{
    auto bottom_right = blob.front();
    std::for_each(std::next(blob.begin()), blob.end(),
        [&bottom_right](auto point)
        {
            if(point.x > bottom_right.x)
            {
                bottom_right.x = point.x;
            }

            if(point.y > bottom_right.y)
            {
                bottom_right.y = point.y;
            }
        });

    return bottom_right;
}

std::vector<cv::Point> get_bottom_rights(const Blobs& blobs)
{
    auto bottom_rights = std::vector<cv::Point>();
    bottom_rights.reserve(blobs.size());

    std::transform(blobs.begin(), blobs.end(),
                   std::back_inserter(bottom_rights),
                   get_bottom_right);

    return bottom_rights;
}

cv::Size calc_size(cv::Point top_left, cv::Point bottom_right)
{
    assert(top_left.x <= bottom_right.x);
    assert(top_left.y <= bottom_right.y);

    const auto width = bottom_right.x - top_left.x;
    const auto height = bottom_right.y - top_left.y;

    return cv::Size{width, height};
}

std::vector<cv::Size> calc_sizes(const std::vector<cv::Point>& top_lefts,
                                 const std::vector<cv::Point>& bottom_rights)
{
    assert(top_lefts.size() == bottom_rights.size());

    auto sizes = std::vector<cv::Size>();
    sizes.reserve(top_lefts.size());

    for(auto i = 0; i < top_lefts.size(); ++i)
    {
        sizes.emplace_back(calc_size(top_lefts[i], bottom_rights[i]));
    }

    return sizes;
}

void shift_blob(Blob& blob, cv::Point top_left)
{
    std::for_each(blob.begin(), blob.end(),
        [&top_left](auto& point)
        {
            point.x -= top_left.x;
            point.y -= top_left.y;
        });
}

void shift_blobs(Blobs& blobs, const std::vector<cv::Point>& top_lefts)
{
    assert(blobs.size() == top_lefts.size());

    for(auto i = 0; i < blobs.size(); ++i)
    {
        shift_blob(blobs[i], top_lefts[i]);
    }
}

std::vector<cv::Mat> make_mats(const std::vector<cv::Size>& sizes)
{
    auto mats = std::vector<cv::Mat>();
    mats.reserve(sizes.size());

    std::transform(sizes.begin(), sizes.end(),
                   std::back_inserter(mats),
                   [](auto& size) { return cv::Mat(size, CV_8UC1); });

    return mats;
}

void transform_blob_to_mat(const Blob& blob, cv::Mat& mat)
{
    for(const auto point : blob)
    {
        assert(is_point_valid(mat, point));
        set_point(mat, point);
    }
}

void transform_blobs_to_mats(const std::vector<Blob>& blobs, std::vector<cv::Mat>& mats)
{
    assert(blobs.size() == mats.size());

    auto blob_it = blobs.begin();
    for(auto& mat : mats)
    {
        transform_blob_to_mat(*(blob_it++), mat);
    }
}

Segments find_segments(cv::Mat& image)
{
    auto blobs = find_blobs(image);
    const auto top_lefts = get_top_lefts(blobs);
    const auto bottom_rights = get_bottom_rights(blobs);

    const auto sizes = calc_sizes(top_lefts, bottom_rights);
    auto mats = make_mats(sizes);

    shift_blobs(blobs, top_lefts);
    transform_blobs_to_mats(blobs, mats);

    return {std::move(top_lefts), std::move(mats)};
}
