#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief Tests, if point is valid and belongs to image
 * It checks, if point's coordinates are greater or equal to zero
 *  and if they are inside an image
 * @param img
 * @param point
 *
 * @return
 */
inline bool is_point_valid(const cv::Mat& img, cv::Point point) noexcept
{
    return (point.x >= 0
        && point.y >= 0
        && point.y < img.rows
        && point.x < img.cols);
}

/**
 * @brief Checks, if in binary image given point has non-zero value
 *
 * @param img
 * @param point
 *
 * @return
 */
inline bool is_point_set(const cv::Mat_<uchar>& img, cv::Point point) noexcept
{
    return (img(point) != 0);
}

/**
 * @brief Clears in binary image given point by setting its value to zero
 *
 * @param img
 * @param point
 */
inline void clear_point(cv::Mat_<uchar>& img, cv::Point point)
{
    img(point) = 0;
}

/**
 * @brief Sets in binary image given point by setting its value to 255
 *
 * @param img
 * @param point
 */
inline void set_point(cv::Mat_<uchar>& img, cv::Point point)
{
    img(point) = 255;
}

template<typename T, std::size_t N>
cv::_InputArray adapt_array(const std::array<T, N>& array)
{
	return cv::_InputArray{array.data(), static_cast<int>(array.size())};
}
