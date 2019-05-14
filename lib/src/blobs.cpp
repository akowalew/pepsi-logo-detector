#include "blobs.hpp"

#include "utility.hpp"

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

static std::optional<cv::Point> find_next_point_at(const cv::Mat& img, cv::Point point)
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

static std::optional<cv::Point> find_next_point_in_blob(const cv::Mat& img, const Blob& blob)
{
    CV_Assert(img.depth() == CV_8U);
    CV_Assert(img.channels() == 1);

    for(const auto& point : blob)
    {
        const auto next_point = find_next_point_at(img, point);
        if(next_point)
        {
            return next_point;
        }
    }

    return {};
}

Blob find_blob_at(cv::Mat& img, cv::Point first)
{
    CV_Assert(img.depth() == CV_8U);
    CV_Assert(img.channels() == 1);
    CV_Assert(is_point_valid(img, first));
    CV_Assert(is_point_set(img, first));

    auto point = first;
    auto blob = Blob();
    blob.reserve(1024);

    while(true)
    {
        clear_point(img, point);
        blob.emplace_back(point);

        if(const auto next_point = find_next_point_at(img, point); next_point)
        {
            point = *next_point;
            continue;
        }

        if(const auto next_point = find_next_point_in_blob(img, blob); next_point)
        {
            point = *next_point;
            continue;
        }

        break; // Next point not found, break
    }

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
            if(*ptr != 0)
            {
                blobs.push_back(find_blob_at(img, point));
            }

            ptr++;
        }
    }

    blobs.shrink_to_fit();
    return blobs;
}
