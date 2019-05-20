#include "catch2/catch.hpp"

#include "core.hpp"

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

SCENARIO("Color images can be thresholded from both sides", "[threshold]")
{
	GIVEN("Image with all zeros")
	{
		const cv::Mat_<cv::Vec3b> src = cv::Mat_<cv::Vec3b>::zeros(8, 10);

		WHEN("Thresholding in range [0,0,0] - [255,255,255]")
		{
			const auto min = cv::Vec3b{0, 0, 0};
			const auto max = cv::Vec3b{255, 255, 255};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("All pixels should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::ones(dst.size())*255;
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Thresholding in range [0,0,0] - [0,0,0]")
		{
			const auto min = cv::Vec3b{0, 0, 0};
			const auto max = cv::Vec3b{0, 0, 0};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("All pixels should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::ones(dst.size())*255;
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Thresholding in range [1,1,1] - [255,255,255]")
		{
			const auto min = cv::Vec3b{1, 1, 1};
			const auto max = cv::Vec3b{255, 255, 255};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("No pixel should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::zeros(dst.size());
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Image with all 255's")
	{
		auto src = cv::Mat_<cv::Vec3b>{8, 10};
		for(auto& v : src)
		{
			v = cv::Vec3b{255, 255, 255};
		}

		WHEN("Thresholding in range [0,0,0] - [255,255,255]")
		{
			const auto min = cv::Vec3b{0, 0, 0};
			const auto max = cv::Vec3b{255, 255, 255};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("All pixels should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::ones(dst.size())*255;
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Thresholding in range [0,0,0] - [254,254,254]")
		{
			const auto min = cv::Vec3b{0, 0, 0};
			const auto max = cv::Vec3b{254, 254, 254};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("No pixel should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::zeros(dst.size());
				REQUIRE(images_equal(dst, target));
			}
		}

		WHEN("Thresholding in range [255,255,255] - [255,255,255]")
		{
			const auto min = cv::Vec3b{254, 254, 254};
			const auto max = cv::Vec3b{255, 255, 255};
			auto dst = cv::Mat_<uchar>{src.size()};
			threshold(src, dst, min, max);

			THEN("All pixels should be marked")
			{
				cv::Mat_<uchar> target = cv::Mat_<uchar>::ones(dst.size())*255;
				REQUIRE(images_equal(dst, target));
			}
		}
	}
}

SCENARIO("Images can be bitwise OR'ed", "[bitwise_or]")
{
	const auto size = cv::Size{8, 10};

	GIVEN("Images with zeros")
	{
		const cv::Mat_<uchar> src1 = cv::Mat_<uchar>::zeros(size);
		const cv::Mat_<uchar> src2 = cv::Mat_<uchar>::zeros(size);

		WHEN("ORing bitwise")
		{
			auto dst = cv::Mat_<uchar>{size};
			bitwise_or(src1, src2, dst);

			THEN("All pixels should be zero")
			{
				const cv::Mat_<uchar> target = cv::Mat_<uchar>::zeros(size);
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Image with zeros and image with 255's")
	{
		const cv::Mat_<uchar> src1 = cv::Mat_<uchar>::zeros(size);
		const cv::Mat_<uchar> src2 = cv::Mat_<uchar>::ones(size) * 255;

		WHEN("ORing bitwise")
		{
			auto dst = cv::Mat_<uchar>{size};
			bitwise_or(src1, src2, dst);

			THEN("All pixels should be marked")
			{
				const cv::Mat_<uchar> target = cv::Mat_<uchar>::ones(size) * 255;
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Same non-zero images")
	{
		auto src = cv::Mat_<uchar>{size};

		// Generate some non-zero image
		auto i = 0;
		for(auto& v : src)
		{
			v = (++i % 2) ? 255 : 0;
		}

		WHEN("ORing bitwise")
		{
			auto dst = cv::Mat_<uchar>{size};
			bitwise_or(src, src, dst);

			THEN("Result will be the same as source")
			{
				const auto& target = dst;
				REQUIRE(images_equal(dst, target));
			}
		}
	}

	GIVEN("Bitwise complementary images")
	{
		auto src1 = cv::Mat_<uchar>{size};
		auto src2 = cv::Mat_<uchar>{size};

		auto src1_it = src1.begin();
		auto src2_it = src2.begin();
		const auto src1_end = src1.end();
		while(src1_it != src1_end)
		{
			auto value = static_cast<uchar>(rand() % 255);
			auto complementary = static_cast<uchar>(~value);

			*(src1_it++) = value;
			*(src2_it++) = complementary;
		}

		WHEN("ORing bitwise")
		{
			auto dst = cv::Mat_<uchar>{size};
			bitwise_or(src1, src2, dst);

			THEN("All pixels should be marked")
			{
				const auto target = cv::Mat_<uchar>::ones(size) * 255;
				REQUIRE(images_equal(dst, target));
			}
		}
	}
}
