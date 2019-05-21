#include "catch2/catch.hpp"

#include "morpho.hpp"
#include "core.hpp"

namespace {

void debug_binary(const cv::Mat_<uchar>& binary)
{
	for(auto y = 0; y < binary.rows; ++y)
	{
		for(auto x = 0; x < binary.cols; ++x)
		{
			putchar(binary(y, x) ? '1' : '0');
		}
		putchar('\n');
	}
	putchar('\n');
}

void rectangle_centered(cv::Mat_<uchar>& img, cv::Size size, uchar color, int thickness = 1)
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

} //

SCENARIO("Morphological operations can be applied to binary images", "[morpho]")
{
	const auto size = cv::Size{8, 8};
	const auto ncols = size.width;
	const auto nrows = size.height;
	const auto kernel = cv::Mat_<uchar>{cv::Size{3, 3}, 255};
	auto dst = cv::Mat_<uchar>{size};

	GIVEN("Black image")
	{
		const auto src = cv::Mat_<uchar>{size, 0};

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("Image should be also black")
			{
				const auto target = cv::Mat_<uchar>{size, 0};
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("Image should be also black")
			{
				const auto target = cv::Mat_<uchar>{size, 0};
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("White image")
	{
		const auto src = cv::Mat_<uchar>{size, 255};

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("Image should be also white")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("Image should be also white")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Checker image")
	{
		auto src = cv::Mat_<uchar>{size, 0};
		for(auto y = 0; y < nrows; ++y)
		{
			for(auto x = 0; x < ncols; ++x)
			{
				if(((y + x) % 2) == 0)
				{
					src(y, x) = 255;
				}
			}
		}

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("Result image should be black")
			{
				auto target = cv::Mat_<uchar>{size, 0};
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("White image should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Black image with 4x4 white square in center")
	{
		auto src = cv::Mat_<uchar>{size, 0};
		rectangle_centered(src, cv::Size{4, 4}, 255, CV_FILLED);

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("Black image with 2x2 white square in center should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 0};
				rectangle_centered(target, cv::Size{2, 2}, 255, CV_FILLED);
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("Black image with 6x6 white square in center should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 0};
				rectangle_centered(target, cv::Size{6, 6}, 255, CV_FILLED);
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Black image with 4xWidth horizontal white strip")
	{
		auto src = cv::Mat_<uchar>{size, 0};
		rectangle_centered(src, cv::Size{ncols, 4}, 255, CV_FILLED);

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("Black image with 2xWidth horizontal white strip should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 0};
				rectangle_centered(target, cv::Size{ncols, 2}, 255, CV_FILLED);

				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("Black image with 6xWidth horizontal white strip should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 0};
				rectangle_centered(target, cv::Size{ncols, 6}, 255, CV_FILLED);
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("White image with 4x4 black square in center")
	{
		auto src = cv::Mat_<uchar>{size, 255};
		rectangle_centered(src, cv::Size{4, 4}, 0, CV_FILLED);

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("White image with 6x6 black square in center should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				rectangle_centered(target, cv::Size{6, 6}, 0, CV_FILLED);

				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("White image with 2x2 black square in center should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				rectangle_centered(target, cv::Size{2, 2}, 0, CV_FILLED);
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("White image with horizontal black strip 4xWidth")
	{
		auto src = cv::Mat_<uchar>{size, 255};
		cv::rectangle(src, {0, nrows/2-2}, {ncols-1, nrows/2+1}, 0, CV_FILLED);

		WHEN("Eroding it with square 3x3 kernel")
		{
			erode(src, dst, kernel);

			THEN("White image with horizontal 6xWidth black strip should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				rectangle_centered(target, cv::Size{ncols, 6}, 0, CV_FILLED);

				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Dilating it with square 3x3 kernel")
		{
			dilate(src, dst, kernel);

			THEN("White image with 2xWidth horizontal black strip should be returned")
			{
				auto target = cv::Mat_<uchar>{size, 255};
				rectangle_centered(target, cv::Size{ncols, 2}, 0, CV_FILLED);
				REQUIRE(images_equal(dst, target));
			}
		}
	}
}
