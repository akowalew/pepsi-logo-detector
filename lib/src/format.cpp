#include "format.hpp"

#include <opencv2/imgproc.hpp>

namespace {

inline uchar calc_hue(uchar blue, uchar green, uchar red, uchar max, uchar diff)
{
	if(diff == 0)
	{
		assert(red == green && green == blue);
		return 0;
	}

	double hue = 60.0;
	const auto den = static_cast<double>(diff);
	if(max == red)
	{
		hue *= (0 + (green - blue) / den);
	}
	else if(max == green)
	{
		hue *= (2 + (blue - red) / den);
	}
	else
	{
		assert(max == blue);
		hue *= (4 + (red - green) / den);
	}

	if(hue < 0)
	{
		hue += 360;
	}

	// Target hue must be divided by two to fit in [0, 180] degree range
	return static_cast<uchar>(hue / 2);
}

inline uchar calc_saturation(uchar max, uchar diff)
{
	if(max == 0)
	{
		return 0;
	}

	return static_cast<uchar>((diff * 255.0) / max);
}

} // namespace

void bgr2hsv(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<cv::Vec3b>& dst)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.isContinuous());
	CV_Assert(dst.isContinuous());

	auto src_it = src.data;
	auto dst_it = dst.data;
	const auto dst_end = dst.dataend;
	while(dst_it != dst_end)
	{
		const auto blue = *(src_it++);
		const auto green = *(src_it++);
		const auto red = *(src_it++);

		const auto min = std::min({blue, green, red});
		const auto max = std::max({blue, green, red});

		const auto diff = (max - min);
		const auto hue = calc_hue(blue, green, red, max, diff);
		const auto saturation = calc_saturation(max, diff);
		const auto value = max;

		*(dst_it++) = hue;
		*(dst_it++) = saturation;
		*(dst_it++) = value;
	}
}
