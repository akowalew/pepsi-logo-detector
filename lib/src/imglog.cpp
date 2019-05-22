#include "imglog.hpp"

#include <opencv2/highgui.hpp>

namespace imglog {

namespace {

bool g_enabled = false;

} // namespace

void enable() noexcept
{
	g_enabled = true;
}

void disable() noexcept
{
	g_enabled = false;
}

bool enabled() noexcept
{
	return g_enabled;
}

void log(const char* img_name, const cv::Mat& img)
{
	if(g_enabled)
	{
		cv::imshow(img_name, img);
		cv::moveWindow(img_name, 0, 0);
	}
}

} // namespace imglog
