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

template<typename T>
void clamp(T& t, T min, T max)
{
    if(t < min)
    {
        t = min;
    }
    else if(t > max)
    {
        t = max;
    }
}

void filter_image(const cv::Mat3b& src, cv::Mat3b& dst, const cv::Mat1f& kernel)
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

    const auto anchor_x = (width/2);
    const auto anchor_y = (height/2);

    auto dst_ptr = dst.data;
    auto src_ptr_start = (src.data - 3*((anchor_y*ncols) + anchor_x));
    for(auto y = 0; y < nrows; ++y)
    {
        for(auto x = 0; x < ncols; ++x, src_ptr_start += 3)
        {
            auto accu = cv::Vec3f::all(0.0f);
            auto kernel_ptr = reinterpret_cast<float*>(kernel.data);
            auto src_ptr = src_ptr_start;
            for(auto ky = 0; ky < height; ++ky)
            {
                for(auto kx = 0; kx < width; ++kx, ++kernel_ptr, src_ptr += 3)
                {
                    const auto src_y = y + ky - anchor_y;
                    const auto src_x = x + kx - anchor_x;

                    if(src_y < 0 || src_y >= nrows || src_x < 0 || src_x >= ncols)
                    {
                        continue;
                    }

                    assert(src_y >= 0 && src_y < nrows);
                    assert(src_x >= 0 && src_x < ncols);
                    assert(src_ptr >= src.data && src_ptr+2 < src.dataend);
                    const auto src_v0 = *(src_ptr);
                    const auto src_v1 = *(src_ptr + 1);
                    const auto src_v2 = *(src_ptr + 2);

                    assert(ky >= 0 && ky < height);
                    assert(kx >= 0 && kx < width);
                    assert((uchar*)kernel_ptr >= kernel.data && (uchar*)kernel_ptr < kernel.dataend);
                    const auto k_v = *kernel_ptr;

                    accu[0] += (k_v * src_v0);
                    accu[1] += (k_v * src_v1);
                    accu[2] += (k_v * src_v2);
                }

                src_ptr += (3*(ncols - width));
            }

            clamp(accu[0], 0.0f, 255.0f);
            clamp(accu[1], 0.0f, 255.0f);
            clamp(accu[2], 0.0f, 255.0f);

            assert(dst_ptr >= dst.data && dst_ptr+2 < dst.dataend);
            *(dst_ptr++) = static_cast<uchar>(accu[0]);
            *(dst_ptr++) = static_cast<uchar>(accu[1]);
            *(dst_ptr++) = static_cast<uchar>(accu[2]);
        }
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
