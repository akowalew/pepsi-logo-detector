#include "morpho.hpp"

namespace {



} // namespace

void dilate(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
            const cv::Mat_<uchar>& kernel)
{
	cv::dilate(src, dst, kernel);
}

void erode(const cv::Mat_<uchar>& src, cv::Mat_<uchar>& dst,
           const cv::Mat_<uchar>& kernel)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.isContinuous());
	CV_Assert(dst.isContinuous());
	CV_Assert(kernel.rows % 2 == 1);
	CV_Assert(kernel.cols % 2 == 1);

	const auto ncols = src.cols;
	const auto nrows = src.rows;

	const auto half_width = (kernel.cols / 2);
	const auto half_height = (kernel.rows / 2);

	const auto max_col = (ncols-half_width-1);
	const auto max_row = (nrows-half_height-1);

	auto first_j = 0;
	auto last_j = half_height;
	auto first_i = 0;
	auto last_i = half_width;

	auto src_ptr = src.data;
	auto dst_ptr = dst.data;
	for(auto y = 0; y < nrows; ++y)
	{
		for(auto x = 0; x < ncols; ++x)
		{
			// const auto first_j = std::max(0, y - half_height);
			// const auto last_j = std::min(nrows, y + half_height);
			// const auto first_i = std::max(0, x - half_width);
			// const auto last_i = std::min(ncols, y + half_width);

			auto min = src(first_j, first_i);
			for(auto j = first_j; j < last_j; ++j)
			{
				for(auto i = first_i; i < last_i; ++i)
				{
					const auto value = src(j, i);
					if(value < min)
					{
						min = value;
					}
				}
			}

			first_i = ((x > half_width) ? (first_i + 1) : 0);
			last_i = ((x < max_col) ? (last_i + 1) : ncols);

			*(dst_ptr++) = min;
		}

		first_j = ((y > half_height) ? (first_j + 1) : 0);
		last_j = ((y < max_row) ? (last_j + 1) : nrows);
	}
}
