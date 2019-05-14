#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using Point = cv::Point;

using Points = std::vector<cv::Point>;

using PointsSet = std::vector<Points>;

using Rect = cv::Rect;

using Rects = std::vector<cv::Rect>;

Rect calc_bounding_rect(const Points& points);

Rects calc_bounding_rects(const PointsSet& points_set);

void shift_points(Points& points, Point p);

void shift_points_set(PointsSet& points_set, const Points& points);
