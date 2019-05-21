#include "morpho.hpp"

#include <limits>

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
	cv::erode(src, dst, kernel);
}
// {
// 	CV_Assert(src.size() == dst.size());
// 	CV_Assert(src.isContinuous());
// 	CV_Assert(dst.isContinuous());
// 	CV_Assert(kernel.rows % 2 == 1);
// 	CV_Assert(kernel.cols % 2 == 1);
// 	CV_Assert(kernel.isContinuous());

// 	const auto ncols = src.cols;
// 	const auto nrows = src.rows;

// 	const auto width = kernel.cols;
// 	const auto height = kernel.rows;

// 	const auto half_width = (width / 2);
// 	const auto half_height = (height / 2);

// 	const auto max_col = (ncols-half_width);
// 	const auto max_row = (nrows-half_height);

// 	auto first_j = 0;
// 	auto last_j = half_height;
// 	auto first_i = 0;
// 	auto last_i = half_width;

// 	auto dst_ptr = (dst.data + (half_height * ncols) + half_width);
// 	for(auto y = half_height; y < max_row; ++y)
// 	{
// 		const auto first_j = (y - half_height);
// 		const auto last_j = (y + half_height + 1);

// 		for(auto x = half_width; x < max_col; ++x)
// 		{
// 			const auto first_i = (x - half_width);
// 			const auto last_i = (x + half_width + 1);

// 			auto kernel_ptr = kernel.data;
// 			auto src_ptr = (src.data + (first_j * ncols) + first_i);
// 			auto min = std::numeric_limits<uchar>::max();
// 			for(auto j = first_j; j < last_j; ++j)
// 			{
// 				for(auto i = first_i; i < last_i; ++i)
// 				{
// 					const auto value = *(src_ptr++);
// 					const auto is_kernel_pixel_set = (*(kernel_ptr++) != 0);
// 					if(is_kernel_pixel_set && (value < min))
// 					{
// 						min = value;
// 					}
// 				}

// 				src_ptr += (ncols - width);
// 			}

// 			*(dst_ptr++) = min;
// 		}

// 		dst_ptr += (width - 1);
// 	}
// }

