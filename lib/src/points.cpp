#include "points.hpp"

Rect calc_bounding_rect(const Points& points)
{
    auto top_left = points.front();
    auto bottom_right = points.front();
    std::for_each(std::next(points.begin()), points.end(),
        [&top_left, &bottom_right](const auto point)
        {
            if(point.x < top_left.x)
            {
                top_left.x = point.x;
            }

            if(point.y < top_left.y)
            {
                top_left.y = point.y;
            }

            if(point.x > bottom_right.x)
            {
                bottom_right.x = point.x;
            }

            if(point.y > bottom_right.y)
            {
                bottom_right.y = point.y;
            }
        });

    return Rect{top_left, bottom_right};
}

Rects calc_bounding_rects(const PointsSet& points_set)
{
    auto bounding_rects = std::vector<Rect>(points_set.size());

    auto points_it = points_set.begin();
    for(auto& bounding_rect : bounding_rects)
    {
    	const auto& points = *(points_it++);
        bounding_rect = calc_bounding_rect(points);
    }

    return bounding_rects;
}

void shift_points(Points& points, Point p)
{
    std::for_each(points.begin(), points.end(),
    	[&p](auto& point)
    	{
    		point -= p;
    	});
}

void shift_points_set(PointsSet& points_set, const Points& points)
{
    assert(points_set.size() == points.size());

    auto point_it = points.begin();
    for(auto& points : points_set)
    {
    	const auto p = *(point_it++);
    	shift_points(points, p);
    }
}
