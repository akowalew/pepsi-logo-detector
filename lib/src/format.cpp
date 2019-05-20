#include "format.hpp"

#include <opencv2/imgproc.hpp>

void bgr2hsv(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<cv::Vec3b>& dst)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.channels() == 3);
	CV_Assert(dst.channels() == 3);

	cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
}
