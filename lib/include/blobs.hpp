#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

#include "types.hpp"

using Blob = std::vector<cv::Point>;

using Blobs = std::vector<Blob>;

using BlobArea = int;

using BlobAreaRange = ValueRange<BlobArea>;

Blob find_blob_at(cv::Mat_<uchar>& img, cv::Point first);

Blobs find_blobs(cv::Mat_<uchar>& img);
