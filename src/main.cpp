#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    // Create a gui window:
    cv::namedWindow("Output", 1);

    // Initialize a 120x350 matrix of black pixels:
    cv::Mat output = cv::Mat::zeros(120, 350, CV_8UC3);

    // Write text on the matrix:
    cv::putText(output,
                "Hello World :)",
                cv::Point(15, 70),
                cv::FONT_HERSHEY_PLAIN,
                3,
                cv::Scalar(0, 255, 0),
                4);

    // Display the image
    cv::imshow("Output", output);

    // Wait for the user to press any key:
    cv::waitKey(0);
}
