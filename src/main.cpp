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

    // Resize image to lower resolution
    const auto size = cv::Size{1024, 576};
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

    // Extract Blue part of logo
    auto extracted_mask = cv::Mat(size, CV_8UC1);
    auto lower_bound = std::vector<uchar>{std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4])};
    auto upper_bound = std::vector<uchar>{std::stoi(argv[5]), std::stoi(argv[6]), std::stoi(argv[7])};
    cv::inRange(img_hsv, lower_bound, upper_bound, extracted_mask);
    cv::imshow("Extracted mask", extracted_mask);

    // Wait for the user to press any key
    cv::waitKey(0);
}
