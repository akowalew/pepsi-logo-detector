#include <cstdio>

#include <array>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int /*argc*/, char** argv)
{
    // Read image and show it
    auto img_color_orig = cv::imread(argv[1], cv::IMREAD_COLOR);
    const auto size = img_color_orig.size();

    // Resize image to lower resolution
    auto img_color = cv::Mat{};
    cv::resize(img_color_orig, img_color, size);
    // cv::imshow("Color image", img_color);

    // Convert image to HSV
    const auto img_hsv = cv::Mat(size, CV_8UC3);
    const auto code = cv::COLOR_BGR2HSV;
    cv::cvtColor(img_color, img_hsv, code);

    // Split HSV image into separate components
    std::array<cv::Mat, 3> hsv_comps;
    cv::split(img_hsv, hsv_comps.begin());
    // cv::imshow("Hue", hsv_comps[0]);
    // cv::imshow("Saturation", hsv_comps[1]);
    // cv::imshow("Value", hsv_comps[2]);

    // NOTE: OpenCV denotes Hue value in range 0..180 (which corresponds to 0..360 degrees)

    // Set blue part extractor thresholds
    const uchar blue_low_h = 100;
    const uchar blue_low_s = 100;
    const uchar blue_low_v = 20;
    const auto blue_lower_bound = std::vector<uchar>{blue_low_h, blue_low_s, blue_low_v};

    const uchar blue_high_h = 132;
    const uchar blue_high_s = 255;
    const uchar blue_high_v = 255;
    const auto blue_upper_bound = std::vector<uchar>{blue_high_h, blue_high_s, blue_high_v};

    // Extract blue part of logo
    auto blue_mask = cv::Mat(size, CV_8UC1);
    cv::inRange(img_hsv, blue_lower_bound, blue_upper_bound, blue_mask);
    cv::imshow("Blue mask", blue_mask);

    // Red component is splitted into two parts. Left is in Hue=0..15 and Right is in Hue=165..180,
    //  so we have to calculate two thresholds and join results

    // Set left red part extractor thresholds
    const uchar left_red_low_h = 0;
    const uchar left_red_low_s = 150;
    const uchar left_red_low_v = 50;
    const auto left_red_lower_bound = std::vector<uchar>{left_red_low_h, left_red_low_s, left_red_low_v};

    const uchar left_red_high_h = 10;
    const uchar left_red_high_s = 255;
    const uchar left_red_high_v = 255;
    const auto left_red_upper_bound = std::vector<uchar>{left_red_high_h, left_red_high_s, left_red_high_v};

    // Set right red part extractor thresholds
    const uchar right_red_low_h = 165;
    const uchar right_red_low_s = 150;
    const uchar right_red_low_v = 150;
    const auto right_red_lower_bound = std::vector<uchar>{right_red_low_h, right_red_low_s, right_red_low_v};

    const uchar right_red_high_h = 180;
    const uchar right_red_high_s = 255;
    const uchar right_red_high_v = 255;
    const auto right_red_upper_bound = std::vector<uchar>{right_red_high_h, right_red_high_s, right_red_high_v};

    // Extract left red part of logo
    auto left_red_mask = cv::Mat(size, CV_8UC1);
    cv::inRange(img_hsv, left_red_lower_bound, left_red_upper_bound, left_red_mask);
    // cv::imshow("Left red mask", left_red_mask);

    // Extract right red part of logo
    auto right_red_mask = cv::Mat(size, CV_8UC1);
    cv::inRange(img_hsv, right_red_lower_bound, right_red_upper_bound, right_red_mask);
    // cv::imshow("Right red mask", right_red_mask);

    // Join results of both red masks
    auto red_mask = cv::Mat(size, CV_8UC1);
    cv::bitwise_or(left_red_mask, right_red_mask, red_mask);
    cv::imshow("Red mask", red_mask);

    // Wait for the user to press any key
    cv::waitKey(0);
}
