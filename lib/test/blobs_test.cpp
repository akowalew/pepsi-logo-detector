#include "catch2/catch.hpp"

#include <string_view>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "blobs.hpp"

SCENARIO("Blobs can be extracted from the binary image", "[find_blobs]")
{
	GIVEN("Empty image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{8, 8};
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
		const auto size = cv::Size{8, 8};
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

				for(auto y = 0; y < size.height; ++y)
				{
					for(auto x = 0; x < size.width; ++x)
					{
						REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(x, y)) == 1);
					}
				}
			}
		}
	}

	GIVEN("Border image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{8, 8};
		cv::Mat img = cv::Mat::zeros(size, type);

		cv::line(img, cv::Point{0, 0}, cv::Point(img.cols-1, 0), 255, 1);
		cv::line(img, cv::Point{img.cols-1, 0}, cv::Point(img.cols-1, img.rows-1), 255, 1);
		cv::line(img, cv::Point{img.cols-1, img.rows-1}, cv::Point(0, img.rows-1), 255, 1);
		cv::line(img, cv::Point{0, img.rows-1}, cv::Point(0, 0), 255, 1);

		WHEN("Finding blobs")
		{
			const auto blobs = find_blobs(img);

			THEN("One blob should be returned")
			{
				REQUIRE(blobs.size() == 1);
			}

			THEN("Returned blob should have all border pixels")
			{
				const auto& blob = blobs.front();

				REQUIRE(blob.size() == ((2 * img.rows) + (2 * (img.cols - 2))));

				for(auto i = 0; i < img.cols; ++i)
				{
					REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(i, 0)) == 1);
				}

				for(auto i = 0; i < img.cols; ++i)
				{
					REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(i, img.rows-1)) == 1);
				}

				for(auto j = 0; j < img.rows; ++j)
				{
					REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(0, j)) == 1);
				}

				for(auto j = 0; j < img.rows; ++j)
				{
					REQUIRE(std::count(blob.begin(), blob.end(), cv::Point(img.cols-1, j)) == 1);
				}
			}
		}
	}

	GIVEN("Checker image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{8, 8};
		cv::Mat img = cv::Mat::zeros(size, type);

		for(auto y = 0; y < img.rows; ++y)
		{
			for(auto x = 0; x < img.cols; ++x)
			{
				if((x + y) % 2 == 0)
				{
					img.at<uchar>(cv::Point(x, y)) = 255;
				}
			}
		}

		WHEN("Finding blobs")
		{
			const auto blobs = find_blobs(img);

			THEN("One blob should be returned")
			{
				REQUIRE(blobs.size() == 1);
			}

			THEN("Returned blob should contain exact pixels")
			{
				const auto& blob = blobs.front();

				for(auto y = 0; y < img.rows; ++y)
				{
					for(auto x = 0; x < img.cols; ++x)
					{
						if((x + y) % 2 == 0)
						{
							REQUIRE(std::count(blob.begin(), blob.end(), cv::Point{x, y}));
						}
					}
				}
			}
		}
	}

	GIVEN("Corner image")
	{
		const auto type = CV_8UC1;
		const auto size = cv::Size{8, 8};
		cv::Mat img = cv::Mat::zeros(size, type);

		img.at<uchar>(cv::Point{0, 0}) = 255;
		img.at<uchar>(cv::Point{img.cols-1, 0}) = 255;
		img.at<uchar>(cv::Point{img.cols-1, img.rows-1}) = 255;
		img.at<uchar>(cv::Point{0, img.rows-1}) = 255;

		WHEN("Finding blobs")
		{
			const auto blobs = find_blobs(img);

			THEN("Four blobs should be returned")
			{
				REQUIRE(blobs.size() == 4);
			}

			THEN("Each blob should have exactly one point")
			{
				REQUIRE(std::all_of(blobs.begin(), blobs.end(),
									[](const auto& blob) { return (blob.size() == 1); }));
			}

			THEN("Top left corner should be returned")
			{
				const auto is_top_left = [&img](const auto& blob) {
					return (blob.front() == cv::Point{0, 0});
				};
				REQUIRE(std::any_of(blobs.begin(), blobs.end(), is_top_left));
			}

			THEN("Top right corner should be returned")
			{
				const auto is_top_right = [&img](const auto& blob) {
					return (blob.front() == cv::Point{img.cols-1, 0});
				};
				REQUIRE(std::any_of(blobs.begin(), blobs.end(), is_top_right));
			}

			THEN("Bottom right corner should be returned")
			{
				const auto is_bottom_right = [&img](const auto& blob) {
					return (blob.front() == cv::Point{img.cols-1, img.rows-1});
				};
				REQUIRE(std::any_of(blobs.begin(), blobs.end(), is_bottom_right));
			}

			THEN("Bottom left corner should be returned")
			{
				const auto is_bottom_left = [&img](const auto& blob) {
					return (blob.front() == cv::Point{0, img.cols-1});
				};
				REQUIRE(std::any_of(blobs.begin(), blobs.end(), is_bottom_left));
			}
		}
	}
}
