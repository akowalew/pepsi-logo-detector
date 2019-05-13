#include "core.hpp"

#include <opencv2/imgproc.hpp>

void threshold(const cv::Mat& src, cv::Mat& dst,
			   double thresh)
{
	CV_Assert(dst.channels() == 1);
	CV_Assert(src.size() == dst.size());

	const auto max_value = 255;
	const auto method = cv::THRESH_BINARY;
	cv::threshold(src, dst, thresh, max_value, method);
}

void double_threshold(const cv::Mat& src, cv::Mat& dst,
					  cv::InputArray lower_bound, cv::InputArray upper_bound)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(lower_bound.size() == upper_bound.size());
	CV_Assert(lower_bound.rows() == 1);
	CV_Assert(src.channels() == lower_bound.cols());
	CV_Assert(dst.channels() == 1);

	cv::inRange(src, lower_bound, upper_bound, dst);
}

void bitwise_or(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
	CV_Assert((src1.size() == src2.size()) && (src2.size() == dst.size()));
	CV_Assert((src1.channels() == src2.channels()) && (src2.channels() == dst.channels()));

	cv::bitwise_or(src1, src2, dst);
}
