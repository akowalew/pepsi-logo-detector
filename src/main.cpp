#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    // Read image and show it
    auto img_color_orig = cv::imread("assets/pepsi3.jpg", cv::IMREAD_COLOR);

    // Resize image to lower resolution
    const auto size = cv::Size{1024, 576};
    auto img_color = cv::Mat{};
    cv::resize(img_color_orig, img_color, size);
    cv::imshow("Color image", img_color);

    cv::waitKey(0);
    return 0;

    // Convert image to gray
    const auto img_gray = cv::Mat();
    const auto code = cv::COLOR_BGR2GRAY;
    cv::cvtColor(img_color, img_gray, code);
    cv::imshow("Gray image", img_gray);

    // Detect circles
    std::vector<cv::Vec3f> circles;
    const auto method = cv::HOUGH_GRADIENT;
    const auto dp = 2.0;
    const auto min_dist = (img_gray.rows / 4.0);
    HoughCircles(img_gray, circles, method,
                 dp, min_dist, 200, 100);

    printf("Detected %lu circles\n", circles.size());

    // Process circles
    for(const auto& circle : circles)
    {
        const auto cx = cvRound(circle[0]);
        const auto cy = cvRound(circle[1]);
        const auto radius = cvRound(circle[2]);

        const auto center = cv::Point{cx, cy};

        // draw the circle center
        const auto center_radius = 3;
        const auto center_color = cv::Scalar{0, 255, 0};
        const auto center_thickness = cv::FILLED;
        const auto center_line_type = cv::LINE_8;
        const auto center_shift = 0;
        cv::circle(img_color, center, center_radius, center_color, center_thickness, center_line_type, center_shift);

        // draw the circle outline
        const auto outline_color = cv::Scalar{0, 0, 255};
        const auto outline_thickness = 3;
        const auto outline_line_type = cv::LINE_8;
        const auto outline_shift = 0;
        cv::circle(img_color, center, radius, outline_color, outline_thickness, outline_line_type, outline_shift);
    }

    cv::imshow("Circles", img_color);

    // Wait for the user to press any key
    cv::waitKey(0);
}
