#pragma once

#include <opencv2/opencv.hpp>

namespace imglog {

void enable() noexcept;
void disable() noexcept;
bool enabled() noexcept;

void log(const char* img_name, const cv::Mat& img);

} // imglog
