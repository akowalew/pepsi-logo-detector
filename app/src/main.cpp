#include <cstdio>

#include <vector>

#include "CLI/CLI.hpp"

#include "PepsiDetector.hpp"

int main(int argc, char** argv)
{
    CLI::App app{"Detects logo of PEPSI drink in images and marks them"};

    bool verbose{false};
    app.add_flag("-v,--verbose", verbose,
        "Prints diagnostic messages");

    std::string ifile;
    app.add_option("-i,--ifile,ifile", ifile,
                   "Path to existing input file. If not specified, reads from stdin");

    std::string ofile;
    app.add_option("-o,--ofile,ofile", ofile,
                   "Path for output file. If not specified, prints to stdout");

    std::vector<std::uint8_t> blue_min_hsv{107, 100, 20};
    app.add_option("--blue-min-hsv", blue_min_hsv,
                   "Minimum HSV value for blue part of logo")
        ->expected(3)
        ->check(CLI::Range(0, 255));

    std::vector<std::uint8_t> blue_max_hsv{130, 255, 255};
    app.add_option("--blue-max-hsv", blue_max_hsv,
                   "Maximum HSV value for blue part of logo")
        ->expected(3)
        ->check(CLI::Range(0, 255));

    bool filter{false};
    app.add_flag("--filter", filter,
                 "Enable additional filtering after color thresholding step");

    double min_area{1000.0};
    app.add_option("--min-area", min_area,
                   "Minimum area of logo components");

    double max_area{3000.0};
    app.add_option("--max-area", max_area,
                   "Maximum area of logo components");

    double min_malinowska{0.6};
    app.add_option("--min-malinowska", min_malinowska,
                   "Minimum value of Malinowska factor");

    double max_malinowska{0.9};
    app.add_option("--max-malinowska", max_malinowska,
                   "Maximum value of Malinowska factor");

    CLI11_PARSE(app, argc, argv);

    auto detector = PepsiDetector{};
    const auto image = cv::imread(ifile, cv::IMREAD_COLOR);
    const auto logos = detector.find_logos(image);

    printf("Found %lu logos\n", logos.size());
    for(const auto& logo : logos)
    {
        printf("%d %d %d %d\n", logo.x, logo.y, logo.width, logo.height);
    }

    cv::waitKey(0);

    return 0;
}











































































// #include <cmath>
// #include <cstdio>

// #include <algorithm>
// #include <array>
// #include <string>
// #include <vector>

// #include <opencv2/opencv.hpp>
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc.hpp>

// void draw_contours_randomly(const std::vector<std::vector<cv::Point>>& contours, cv::Mat& img)
// {
//     for(auto i = 0; i < contours.size(); ++i)
//     {
//         const auto color = cv::Scalar(rand() & 255, rand() & 255, rand() & 255);
//         const auto thickness = 3;
//         drawContours(img, contours, i, color, thickness);
//     }
// }

// double calc_malinowska_factor(double perimeter, double area)
// {
//     return perimeter / (2 * std::sqrt(M_PI * area)) - 1;
// }

// int main(int /*argc*/, char** argv)
// {
//     // Read image
//     auto img_color = cv::imread(argv[1], cv::IMREAD_COLOR);
//     const auto size = img_color.size();
//     // cv::imshow("Color image", img_color);

//     // Convert image to HSV
//     const auto img_hsv = cv::Mat(size, CV_8UC3);
//     const auto code = cv::COLOR_BGR2HSV;
//     cv::cvtColor(img_color, img_hsv, code);

//     // NOTE: OpenCV denotes Hue value in range 0..180 (which corresponds to 0..360 degrees)

//     // Set blue part extractor thresholds
//     const uchar blue_low_h = 100;
//     const uchar blue_low_s = 100;
//     const uchar blue_low_v = 20;
//     const auto blue_lower_bound = std::vector<uchar>{blue_low_h, blue_low_s, blue_low_v};

//     const uchar blue_high_h = 132;
//     const uchar blue_high_s = 255;
//     const uchar blue_high_v = 255;
//     const auto blue_upper_bound = std::vector<uchar>{blue_high_h, blue_high_s, blue_high_v};

//     // Extract blue part of logo
//     auto blue_mask = cv::Mat(size, CV_8UC1);
//     cv::inRange(img_hsv, blue_lower_bound, blue_upper_bound, blue_mask);
//     cv::imshow("Blue mask", blue_mask);

//     // // Perform closing on blue mask
//     // const auto kernel_shape = cv::MORPH_RECT;
//     // const auto kernel_size = cv::Size{5, 5};
//     // const auto kernel = cv::getStructuringElement(kernel_shape, kernel_size);
//     // cv::erode(blue_mask, blue_mask, kernel);
//     // cv::dilate(blue_mask, blue_mask, kernel);
//     // cv::imshow("Blue mask (filtered)", blue_mask);

//     // Find contours in blue mask
//     std::vector<std::vector<cv::Point>> blue_contours_all;
//     const auto blue_retr_mode = cv::RETR_LIST;
//     const auto blue_approx_method = cv::CHAIN_APPROX_NONE;
//     cv::findContours(blue_mask, blue_contours_all, blue_retr_mode, blue_approx_method);

//     // Filter out blue contours with small area
//     auto blue_contours = std::vector<std::vector<cv::Point>>(blue_contours_all.size());
//     const auto is_blue_part = [](const auto& contour) {
//         const auto area = cv::contourArea(contour);
//         return area >= 1500.0 && area <= 5000.0;
//     };
//     const auto last_blue_it = std::copy_if(blue_contours_all.begin(), blue_contours_all.end(),
//                                            blue_contours.begin(), is_blue_part);
//     const auto blue_contours_count = std::distance(blue_contours.begin(), last_blue_it);
//     blue_contours.resize(blue_contours_count);

//     // Print blue contours perimeters
//     printf("Blue Contours perimeters:\n");
//     const auto print_contour_perimeter = [](const auto& contour) { printf("%lf\n", cv::arcLength(contour, true)); };
//     std::for_each(blue_contours.begin(), blue_contours.end(), print_contour_perimeter);

//     // Print blue contours areas
//     printf("Blue Contours areas:\n");
//     const auto print_contour_area = [](const auto& contour) { printf("%lf\n", cv::contourArea(contour)); };
//     std::for_each(blue_contours.begin(), blue_contours.end(), print_contour_area);

//     // Print Malinowska factors
//     printf("Blue Contours Malinowska factors:\n");
//     const auto print_contour_mf = [](const auto& contour) { printf("%lf\n", calc_malinowska_factor(cv::arcLength(contour, true), cv::contourArea(contour))); };
//     std::for_each(blue_contours.begin(), blue_contours.end(), print_contour_mf);

//     // Draw blue contours
//     auto blue_contours_img = img_color.clone();
//     draw_contours_randomly(blue_contours, blue_contours_img);
//     cv::imshow("Blue contours", blue_contours_img);

//     // Obtain moments of blue parts
//     auto blue_moments = std::vector<cv::Moments>(blue_contours.size());
//     const auto calc_moments = [](const auto& contour) { return cv::moments(contour); };
//     std::transform(blue_contours.begin(), blue_contours.end(), blue_moments.begin(), calc_moments);

//     // Print blue parts moments
//     printf("Blue central moments:\n");
//     const auto print_central_moments = [](const auto& m) {
//         printf("%16.3lf %16.3lf %16.3lf %16.3lf %16.3lf %16.3lf %16.3lf\n", m.mu20, m.mu11, m.mu02, m.mu30, m.mu21, m.mu12, m.mu03);
//     };
//     std::for_each(blue_moments.begin(), blue_moments.end(), print_central_moments);

//     cv::waitKey(0);
//     return 0;

//     // Red component is splitted into two parts. Left is in Hue=0..15 and Right is in Hue=165..180,
//     //  so we have to calculate two thresholds and join results

//     // Set left red part extractor thresholds
//     const uchar left_red_low_h = 0;
//     const uchar left_red_low_s = 150;
//     const uchar left_red_low_v = 50;
//     const auto left_red_lower_bound = std::vector<uchar>{left_red_low_h, left_red_low_s, left_red_low_v};

//     const uchar left_red_high_h = 10;
//     const uchar left_red_high_s = 255;
//     const uchar left_red_high_v = 255;
//     const auto left_red_upper_bound = std::vector<uchar>{left_red_high_h, left_red_high_s, left_red_high_v};

//     // Set right red part extractor thresholds
//     const uchar right_red_low_h = 165;
//     const uchar right_red_low_s = 150;
//     const uchar right_red_low_v = 150;
//     const auto right_red_lower_bound = std::vector<uchar>{right_red_low_h, right_red_low_s, right_red_low_v};

//     const uchar right_red_high_h = 180;
//     const uchar right_red_high_s = 255;
//     const uchar right_red_high_v = 255;
//     const auto right_red_upper_bound = std::vector<uchar>{right_red_high_h, right_red_high_s, right_red_high_v};

//     // Extract left red part of logo
//     auto left_red_mask = cv::Mat(size, CV_8UC1);
//     cv::inRange(img_hsv, left_red_lower_bound, left_red_upper_bound, left_red_mask);
//     // cv::imshow("Left red mask", left_red_mask);

//     // Extract right red part of logo
//     auto right_red_mask = cv::Mat(size, CV_8UC1);
//     cv::inRange(img_hsv, right_red_lower_bound, right_red_upper_bound, right_red_mask);
//     // cv::imshow("Right red mask", right_red_mask);

//     // Join results of both red masks
//     auto red_mask = cv::Mat(size, CV_8UC1);
//     cv::bitwise_or(left_red_mask, right_red_mask, red_mask);
//     cv::imshow("Red mask", red_mask);

//     // Find contours in red mask
//     std::vector<std::vector<cv::Point>> red_contours_all;
//     const auto red_retr_mode = cv::RETR_LIST;
//     const auto red_approx_method = cv::CHAIN_APPROX_NONE;
//     cv::findContours(red_mask, red_contours_all, red_retr_mode, red_approx_method);

//     // Filter out red contours with small area
//     auto red_contours = std::vector<std::vector<cv::Point>>(red_contours_all.size());
//     const auto min_red_contour_area = 1000.0;
//     const auto is_red_not_small = [min_red_contour_area](const auto& contour) { return (cv::contourArea(contour) >= min_red_contour_area); };
//     const auto last_red_it = std::copy_if(red_contours_all.begin(), red_contours_all.end(),
//                                            red_contours.begin(), is_red_not_small);
//     const auto red_contours_count = std::distance(red_contours.begin(), last_red_it);
//     red_contours.resize(red_contours_count);

//     printf("Red ontours:\n");
//     for(const auto& contour : red_contours)
//     {
//         printf("%lf\n", cv::contourArea(contour));
//     }

//     // Draw red contours
//     auto red_contours_img = img_color.clone();
//     draw_contours_randomly(red_contours, red_contours_img);
//     cv::imshow("Red contours", red_contours_img);

//     // Wait for the user to press any key
//     cv::waitKey(0);
// }
