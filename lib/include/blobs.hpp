#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using Blob = std::vector<cv::Point>;

using Blobs = std::vector<Blob>;

Blob find_blob_at(cv::Mat& img, cv::Point first);

Blobs find_blobs(cv::Mat& img);
