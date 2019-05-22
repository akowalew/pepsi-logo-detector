#include "drawing.hpp"

void draw_blobs(const Blobs& blobs, cv::Mat_<cv::Vec3b>& img, const cv::Vec3b& color)
{
	cv::Mat_<uchar> mask = cv::Mat_<uchar>::zeros(img.size());
    for(const auto& blob : blobs)
    {
    	for(const auto point : blob)
    	{
    		mask(point) = 255;
    	}
    }

    img.setTo(color, mask);
}

void draw_blobs_randomly(const Blobs& blobs, cv::Mat_<cv::Vec3b>& img)
{
	for(const auto& blob : blobs)
	{
		const auto blue = static_cast<uchar>(rand() % 255);
		const auto green = static_cast<uchar>(rand() % 255);
		const auto red = static_cast<uchar>(rand() % 255);
		const auto color = cv::Vec3b{blue, green, red};

		for(const auto point : blob)
		{
			img(point) = color;
		}
	}
}

void draw_rectangle_centered(cv::Mat_<uchar>& img, cv::Size size, uchar color, int thickness)
{
	const auto ncols = img.cols;
	const auto nrows = img.rows;
	const auto width = size.width;
	const auto height = size.height;
	cv::rectangle(img,
		          {ncols/2 - width/2, nrows/2 - height/2},
		          {ncols/2 + width/2 - 1, nrows/2 + height/2 - 1},
		          color,
		          thickness);
}
