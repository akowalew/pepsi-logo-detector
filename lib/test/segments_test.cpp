#include "catch2/catch.hpp"

#include <string_view>

#include <opencv2/opencv.hpp>

#include "segments.hpp"

TEST_CASE("For empty image no segment should be returned", "[find_segments]")
{
	auto image = cv::Mat(cv::Size(32, 24), CV_8UC1);
	const auto [anchors, mats] = find_segments(image);

	REQUIRE(anchors.empty());
	REQUIRE(mats.empty());
}
