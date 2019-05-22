#include "morpho.hpp"

#include <limits>

void dilate(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
            const cv::Mat_<uchar>& kernel)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.isContinuous());
	CV_Assert(dst.isContinuous());
	CV_Assert(kernel.rows % 2 == 1);
	CV_Assert(kernel.cols % 2 == 1);
	CV_Assert(kernel.isContinuous());
	CV_Assert(&src != &dst);
	CV_Assert(kernel.rows < 256);
	CV_Assert(kernel.cols < 256);

    const auto ncols = src.cols;
    const auto nrows = src.rows;

    const uint8_t width = kernel.cols;
    const uint8_t height = kernel.rows;

    const auto anchor_x = (width/2);
    const auto anchor_y = (height/2);

    auto dst_ptr = dst.data;
    for(auto y = 0; y < nrows; ++y)
    {
        for(auto x = 0; x < ncols; ++x)
        {
			auto max_value = std::numeric_limits<uchar>::min();
			auto kernel_ptr = kernel.data;
            for(auto ky = 0; ky < height; ++ky)
            {
                for(auto kx = 0; kx < width; ++kx, ++kernel_ptr)
                {
                    const auto src_y = y + ky - anchor_y;
                    if(src_y < 0 || src_y >= nrows)
                    {
                        continue;
                    }

                    const auto src_x = x + kx - anchor_x;
                    if(src_x < 0 || src_x >= ncols)
                    {
                        continue;
                    }

                    assert(ky >= 0 && ky < height);
                    assert(kx >= 0 && kx < width);
                    assert(kernel_ptr >= kernel.data && kernel_ptr < kernel.dataend);
                    if(const auto k_v = *kernel_ptr; k_v != 0)
                    {
	                    assert(src_y >= 0 && src_y < nrows);
	                    assert(src_x >= 0 && src_x < ncols);
	                    if(const auto src_v = src(src_y, src_x); src_v > max_value)
	                    {
	                    	max_value = src_v;
	                    }
                    }
                }
            }

            *(dst_ptr++) = max_value;
        }
    }
}

void erode(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
           const cv::Mat_<uchar>& kernel)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.isContinuous());
	CV_Assert(dst.isContinuous());
	CV_Assert(kernel.rows % 2 == 1);
	CV_Assert(kernel.cols % 2 == 1);
	CV_Assert(kernel.isContinuous());
	CV_Assert(&src != &dst);
	CV_Assert(kernel.rows < 256);
	CV_Assert(kernel.cols < 256);

    const auto ncols = src.cols;
    const auto nrows = src.rows;

    const uint8_t width = kernel.cols;
    const uint8_t height = kernel.rows;

    const auto anchor_x = (width/2);
    const auto anchor_y = (height/2);

    auto dst_ptr = dst.data;
    for(auto y = 0; y < nrows; ++y)
    {
        for(auto x = 0; x < ncols; ++x)
        {
			auto min_value = std::numeric_limits<uchar>::max();
			auto kernel_ptr = kernel.data;
            for(auto ky = 0; ky < height; ++ky)
            {
                for(auto kx = 0; kx < width; ++kx, ++kernel_ptr)
                {
                    const auto src_y = y + ky - anchor_y;
                    if(src_y < 0 || src_y >= nrows)
                    {
                        continue;
                    }

                    const auto src_x = x + kx - anchor_x;
                    if(src_x < 0 || src_x >= ncols)
                    {
                        continue;
                    }

                    assert(ky >= 0 && ky < height);
                    assert(kx >= 0 && kx < width);
                    assert(kernel_ptr >= kernel.data && kernel_ptr < kernel.dataend);
                    if(const auto k_v = *kernel_ptr; k_v != 0)
                    {
	                    assert(src_y >= 0 && src_y < nrows);
	                    assert(src_x >= 0 && src_x < ncols);
	                    if(const auto src_v = src(src_y, src_x); src_v < min_value)
	                    {
	                    	min_value = src_v;
	                    }
                    }
                }
            }

            assert(dst_ptr >= dst.data && dst_ptr < dst.dataend);
            *(dst_ptr++) = min_value;
        }
    }
}
