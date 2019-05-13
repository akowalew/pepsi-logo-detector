#include "catch2/catch.hpp"

#include <string_view>

#include <opencv2/opencv.hpp>

#include "PepsiDetector.hpp"

auto find_logos(std::string_view image_file)
{
    auto detector = PepsiDetector{};
    auto image = cv::imread(image_file.data(), cv::IMREAD_COLOR);
    return detector.find_logos(image);
}

auto count_logos(std::string_view image_file)
{
	return find_logos(image_file).size();
}

TEST_CASE("Correct number of logos should be returned", "[find_logos]")
{
    REQUIRE(count_logos("assets/pepsi1.jpg") == 3);
    REQUIRE(count_logos("assets/pepsi4.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi6.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi7.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi8.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi9.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi10.jpg") == 1);
    REQUIRE(count_logos("assets/pepsi11.jpg") == 1);
}
