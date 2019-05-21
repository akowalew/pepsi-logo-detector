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

	const auto ncols = src.cols;
	const auto nrows = src.rows;

	const auto width = kernel.cols;
	const auto height = kernel.rows;

	const auto half_width = (width / 2);
	const auto half_height = (height / 2);

	auto dst_ptr = dst.data;
	for(auto y = 0; y < nrows; ++y)
	{
		for(auto x = 0; x < ncols; ++x)
		{
			const auto begin_j = std::max(0, y - half_height);
			const auto end_j = std::min(nrows, y + half_height + 1);

			const auto begin_i = std::max(0, x - half_width);
			const auto end_i = std::min(ncols, x + half_width + 1);

			auto max_value = std::numeric_limits<uchar>::min();
			auto kernel_ptr = kernel.data;
			auto src_ptr = (src.data + (begin_j * ncols) + begin_i);
			for(auto j = begin_j; j < end_j; ++j)
			{
				for(auto i = begin_i; i < end_i; ++i)
				{
					assert(j >= 0 && j < nrows);
					assert(i >= 0 && i < ncols);
					assert(src_ptr < src.dataend);
					const auto src_value = *(src_ptr++);

					assert(kernel_ptr < kernel.dataend);
					const auto is_kernel_pixel_set = (*(kernel_ptr++) != 0);
					if(is_kernel_pixel_set)
					{
						if(src_value > max_value)
						{
							max_value = src_value;
						}
					}
				}

				src_ptr += (ncols - (end_i - begin_i));
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

	const auto ncols = src.cols;
	const auto nrows = src.rows;

	const auto width = kernel.cols;
	const auto height = kernel.rows;

	const auto half_width = (width / 2);
	const auto half_height = (height / 2);

	auto dst_ptr = dst.data;
	for(auto y = 0; y < nrows; ++y)
	{
		for(auto x = 0; x < ncols; ++x)
		{
			const auto begin_j = std::max(0, y - half_height);
			const auto end_j = std::min(nrows, y + half_height + 1);

			const auto begin_i = std::max(0, x - half_width);
			const auto end_i = std::min(ncols, x + half_width + 1);

			auto min_value = std::numeric_limits<uchar>::max();
			auto kernel_ptr = kernel.data;
			auto src_ptr = (src.data + (begin_j * ncols) + begin_i);
			for(auto j = begin_j; j < end_j; ++j)
			{
				for(auto i = begin_i; i < end_i; ++i)
				{
					assert(j >= 0 && j < nrows);
					assert(i >= 0 && i < ncols);
					assert(src_ptr < src.dataend);
					const auto src_value = *(src_ptr++);

					assert(kernel_ptr < kernel.dataend);
					const auto is_kernel_pixel_set = (*(kernel_ptr++) != 0);
					if(is_kernel_pixel_set)
					{
						if(src_value < min_value)
						{
							min_value = src_value;
						}
					}
				}

				src_ptr += (ncols - (end_i - begin_i));
			}

			*(dst_ptr++) = min_value;
		}
	}
}
