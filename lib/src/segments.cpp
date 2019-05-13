#include "segments.hpp"

#include <cassert>

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

Segment find_segment(cv::Mat& img, cv::Point first)
{
    CV_Assert(img.depth() == 1);
    CV_Assert(img.channels() == 1);
    CV_Assert(img.isContinuous());

    auto x = first.x;
    auto y = first.y;
    auto ptr = reinterpret_cast<uchar*>(img.data) + (y * img.cols) + x;

    auto segment = Segment();
    segment.reserve(1024);

    while(true)
    {
        assert(x < img.rows && x >= 0);
        assert(y < img.cols && y >= 0);
        assert(ptr < img.dataend && ptr >= img.data);

        assert(*ptr); // This pixel should be set
        segment.emplace_back(x, y);
        *ptr = 0; // Turn off that pixel

        bool next_found = false;

        // TODO: Maybe store the pointer instead of points?

        for(auto it = segment.rbegin(); it != segment.rend(); ++it)
        {

        }

        // TODO: stack unwind
        for(const auto shift : Shifts)
        {
            const auto next_ptr = ptr + (shift.sy * img.cols) + shift.sx;
            if(*next_ptr)
            {
                next_found = true;
                ptr = next_ptr;
                // TODO: border case
                x += shift.sx;
                y += shift.sy;
                break;
            }
        }
    }

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
    for(auto y = 0; y < img.rows; ++y)
    {
        for(auto x = 0; x < img.rows; ++x)
        {
            if(*ptr)
            {
                segments.push_back(find_segment(img, cv::Point{x, y}));
            }

            ptr++;
        }
    }

    segments.shrink_to_fit();
    return segments;
}
