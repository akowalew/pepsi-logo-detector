#include "segments.hpp"

#include <cassert>

#include <optional>
#include <stack>

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

std::optional<cv::Point> find_next_point_from(const cv::Mat& img, cv::Point point)
{
    CV_Assert(img.depth() == 1);
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

std::optional<cv::Point> find_next_point_in_segment(const cv::Mat& img, const Segment& segment)
{
    CV_Assert(img.depth() == 1);
    CV_Assert(img.channels() == 1);

    for(const auto& point : segment)
    {
        const auto next_point = find_next_point_from(img, point);
        if(next_point)
        {
            return next_point;
        }
    }

    return {};
}

Segment find_segment(cv::Mat& img, cv::Point first)
{
    CV_Assert(img.depth() == 1);
    CV_Assert(img.channels() == 1);
    CV_Assert(is_point_valid(img, first));
    CV_Assert(is_point_set(img, first));

    auto point = first;
    auto segment = Segment();
    segment.reserve(1024);

    do
    {
        clear_point(img, point);
        segment.emplace_back(point);

        if(const auto next_point = find_next_point_from(img, point); next_point)
        {
            point = *next_point;
            continue;
        }

        if(const auto next_point = find_next_point_in_segment(img, segment); next_point)
        {
            point = *next_point;
            continue;
        }
    }
    while(false);

    segment.shrink_to_fit();
    return segment;
}

Segments find_segments(cv::Mat& img)
{
    CV_Assert(img.depth() == 1);
	CV_Assert(img.channels() == 1);
    CV_Assert(img.isContinuous());

	auto segments = Segments{};
    segments.reserve(1024); // Typically it should be less than this value

    auto ptr = reinterpret_cast<uchar*>(img.data);
    auto point = cv::Point();
    for(point.y = 0; point.y < img.rows; ++point.y)
    {
        for(point.x = 0; point.x < img.cols; ++point.x)
        {
            if(*ptr)
            {
                segments.push_back(find_segment(img, point));
            }

            ptr++;
        }
    }

    segments.shrink_to_fit();
    return segments;
}
