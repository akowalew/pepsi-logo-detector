#include "catch2/catch.hpp"

#include <string_view>

#include <opencv2/opencv.hpp>

#include "blobs.hpp"

SCENARIO("Blobs can be extracted from the binary image", "[find_blobs]")
{
	GIVEN("Empty image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{32, 24};
		cv::Mat img = cv::Mat::zeros(size, type);

		WHEN("Finding blobs")
		{
			const auto blobs = find_blobs(img);

			THEN("No blob should be returned")
			{
				REQUIRE(blobs.empty());
			}

			THEN("Image should be unmodified")
			{
				REQUIRE(img.depth() == type);
				REQUIRE(img.size() == size);
				REQUIRE(std::all_of(img.begin<uchar>(), img.end<uchar>(),
									[](const auto& value) { return (value == 0); }));
			}
		}
	}

	GIVEN("Filled image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{32, 24};
		cv::Mat img = cv::Mat::ones(size, type);

		WHEN("Finding blobs")
		{
			const auto blobs = find_blobs(img);

			THEN("One blob should be returned")
			{
				REQUIRE(blobs.size() == 1);
			}

			THEN("Returned blob should have all pixels from image")
			{
				const auto& blob = blobs.front();

				REQUIRE(blob.size() == img.total());
				// for(auto y = 0; y < size.height; ++y)
				// {
				// 	for(auto x = 0; x < size.width; ++x)
				// 	{
				// 		REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(x, y)) == 1);
				// 	}
				// }
			}
		}
	}
}
