#include "blobs.hpp"

#include "utility.hpp"

struct Shift
{
    std::int8_t sx;
    std::int8_t sy;
};

static inline cv::Point operator+(cv::Point point, Shift shift)
{
    return {point.x + shift.sx, point.y + shift.sy};
}

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

Blob find_blob_at(cv::Mat_<uchar>& img, cv::Point first)
{
    CV_Assert(is_point_valid(img, first));
    CV_Assert(is_point_set(img, first));

    auto blob = Blob();
    blob.emplace_back(first);
    clear_point(img, first);

    for(auto i = 0; i < blob.size(); ++i)
    {
        auto point = blob[i];

        for(const auto shift : Shifts)
        {
            const auto next_point = (point + shift);
            if(is_point_valid(img, next_point) && is_point_set(img, next_point))
            {
                clear_point(img, next_point);
                blob.emplace_back(next_point);
            }
        }
    }

    return blob;
}

Blobs find_blobs(cv::Mat_<uchar>& img)
{
    CV_Assert(img.isContinuous());

	auto blobs = Blobs();
    blobs.reserve(256); // Typically it should be less than this value

    const auto nrows = img.rows;
    const auto ncols = img.cols;

    auto point = cv::Point();
    auto ptr = img.data;
    for(point.y = 0; point.y < nrows; ++point.y)
    {
        for(point.x = 0; point.x < ncols; ++point.x)
        {
            if(*ptr != 0)
            {
                blobs.emplace_back(find_blob_at(img, point));
            }

            ptr++;
        }
    }

    blobs.shrink_to_fit();
    return blobs;
}
