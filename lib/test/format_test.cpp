#include "catch2/catch.hpp"

#include "format.hpp"
#include "core.hpp"

SCENARIO("Images can be converted to other formats", "[bgr2hsv]")
{
	const auto size = cv::Size{8, 10};

	GIVEN("Totally black BGR image")
	{
		const cv::Mat_<cv::Vec3b> bgr = cv::Mat_<cv::Vec3b>::zeros(size);

		WHEN("Converting to HSV")
		{
			auto hsv = cv::Mat_<cv::Vec3b>{size};
			bgr2hsv(bgr, hsv);

			THEN("Image should be zeroed")
			{
				const auto target = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{0, 0, 0}};
				REQUIRE(images_equal(hsv, target));
			}
		}
	}

	GIVEN("Totally white BGR image")
	{
		const auto bgr = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{255, 255, 255}};

		WHEN("Converting to HSV")
		{
			auto hsv = cv::Mat_<cv::Vec3b>{size};
			bgr2hsv(bgr, hsv);

			THEN("Image should be filled with [0, 0, 255]")
			{
				const auto target = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{0, 0, 255}};
				REQUIRE(images_equal(hsv, target));
			}
		}
	}

	GIVEN("Totally blue BGR image")
	{
		const auto bgr = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{255, 0, 0}};

		WHEN("Converting to HSV")
		{
			auto hsv = cv::Mat_<cv::Vec3b>{size};
			bgr2hsv(bgr, hsv);

			THEN("Image should be filled with [120, 255, 255]")
			{
				const auto target = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{120, 255, 255}};
				REQUIRE(images_equal(hsv, target));
			}
		}
	}

	GIVEN("Totally green BGR image")
	{
		const auto bgr = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{0, 255, 0}};

		WHEN("Converting to HSV")
		{
			auto hsv = cv::Mat_<cv::Vec3b>{size};
			bgr2hsv(bgr, hsv);

			THEN("Image should be filled with [60, 255, 255]")
			{
				const auto target = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{60, 255, 255}};
				REQUIRE(images_equal(hsv, target));
			}
		}
	}

	GIVEN("Totally red BGR image")
	{
		const auto bgr = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{0, 0, 255}};

		WHEN("Converting to HSV")
		{
			auto hsv = cv::Mat_<cv::Vec3b>{size};
			bgr2hsv(bgr, hsv);

			THEN("Image should be filled with [0, 255, 255]")
			{
				const auto target = cv::Mat_<cv::Vec3b>{size, cv::Vec3b{0, 255, 255}};
				REQUIRE(images_equal(hsv, target));
			}
		}
	}
}
