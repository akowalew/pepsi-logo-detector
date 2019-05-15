#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

#include "types.hpp"

Rect calc_bounding_rect(const Points& points);

Rects calc_bounding_rects(const PointsSet& points_set);

void shift_points(Points& points, Point p);

void shift_points_set(PointsSet& points_set, const Points& points);
