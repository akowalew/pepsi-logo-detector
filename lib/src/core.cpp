#include "core.hpp"

#include <opencv2/imgproc.hpp>

void threshold(const cv::Mat_<cv::Vec3b>& src, cv::Mat_<uchar>& dst,
			   const ColorRange& color_range)
{
	CV_Assert(src.size() == dst.size());
	CV_Assert(src.isContinuous());
	CV_Assert(dst.isContinuous());

	auto src_ptr = src.data;
    const auto src_end = src.dataend;
    auto dst_ptr = dst.data;

    while(src_ptr != src_end)
    {
    	const auto a = *(src_ptr++);
    	const auto b = *(src_ptr++);
    	const auto c = *(src_ptr++);

    	if(a >= color_range.min[0] && a <= color_range.max[0]
    		&& b >= color_range.min[1] && b <= color_range.max[1]
    		&& c >= color_range.min[2] && c <= color_range.max[2])
    	{
    		*(dst_ptr) = 255;
    	}
    	else
    	{
    		*(dst_ptr) = 0;
    	}

    	++dst_ptr;
    }
}

void bitwise_or(const cv::Mat_<uchar>& src1, const cv::Mat_<uchar>& src2, cv::Mat_<uchar>& dst)
{
	CV_Assert(src1.size() == src2.size());
    CV_Assert(src2.size() == dst.size());
    CV_Assert(src1.isContinuous());
    CV_Assert(src2.isContinuous());
    CV_Assert(dst.isContinuous());

    auto src1_ptr = src1.data;
    auto src2_ptr = src2.data;
    auto dst_ptr = dst.data;
    const auto dst_end = dst.dataend;
    while(dst_ptr != dst_end)
    {
        *(dst_ptr++) = (*(src1_ptr++) | *(src2_ptr++));
    }
}

bool images_equal(const cv::Mat& img1, const cv::Mat& img2)
{
    assert(img1.size() == img2.size());
    assert(img1.channels() == img2.channels());
    assert(img1.depth() == img2.depth());
    assert(img1.isContinuous());
    assert(img2.isContinuous());

    assert(img1.data == img1.datastart);
    return (std::memcmp(img1.data, img2.data, img1.dataend - img1.datastart) == 0);
}
