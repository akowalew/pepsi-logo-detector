#include "moments.hpp"

#include <cassert>
#include <cmath>

static inline SpatialMoment calc_spatial_moment(const Blob& blob, int p, int q)
{
	auto spatial_moment = SpatialMoment{0};
	for(const auto point : blob)
	{
		spatial_moment += (std::pow(point.x, p) * std::pow(point.y, q));
	}

	return spatial_moment;
}

static inline CentralMoment calc_central_moment(const Blob& blob, int p, int q, Centroid centroid)
{
	auto central_moment = CentralMoment{0};
	for(const auto point : blob)
	{
		const auto px = std::pow(point.x - centroid.x, p);
		const auto qy = std::pow(point.y - centroid.y, q);
		central_moment += (px * qy);
	}

	return central_moment;
}

static inline NormalizedMoment calc_normalized_moment(CentralMoment muij, int i, int j, SpatialMoment m00)
{
	const auto num = muij;
	const double den = std::pow(m00, 1 + ((i+j) / 2));
	return (num / den);
}

template<typename T>
constexpr auto sqr(T t) noexcept
{
	return t*t;
}

SpatialMoments calc_spatial_moments(const Blob& blob) noexcept
{
	return SpatialMoments {
		calc_spatial_moment(blob, 0, 0),
		calc_spatial_moment(blob, 1, 0),
		calc_spatial_moment(blob, 0, 1),
		calc_spatial_moment(blob, 2, 0),
		calc_spatial_moment(blob, 1, 1),
		calc_spatial_moment(blob, 0, 2),
		calc_spatial_moment(blob, 3, 0),
		calc_spatial_moment(blob, 2, 1),
		calc_spatial_moment(blob, 1, 2),
		calc_spatial_moment(blob, 0, 3)
	};
}

Centroid calc_centroid(const SpatialMoments& spatial) noexcept
{
	const auto centr_x = (static_cast<double>(spatial.m10) / spatial.m00);
	const auto centr_y = (static_cast<double>(spatial.m01) / spatial.m00);

	return {centr_x, centr_y};
}

CentralMoments calc_central_moments(const Blob& blob, Centroid centroid) noexcept
{
	return CentralMoments {
		calc_central_moment(blob, 2, 0, centroid),
		calc_central_moment(blob, 1, 1, centroid),
		calc_central_moment(blob, 0, 2, centroid),
		calc_central_moment(blob, 3, 0, centroid),
		calc_central_moment(blob, 2, 1, centroid),
		calc_central_moment(blob, 1, 2, centroid),
		calc_central_moment(blob, 0, 3, centroid),
	};
}

NormalizedMoments calc_normalized_moments(const CentralMoments& central_moments, SpatialMoment m00) noexcept
{
	const auto [mu20, mu11, mu02, mu30, mu21, mu12, mu03] = central_moments;

	return NormalizedMoments {
		calc_normalized_moment(mu20, 2, 0, m00),
		calc_normalized_moment(mu11, 1, 1, m00),
		calc_normalized_moment(mu02, 0, 2, m00),
		calc_normalized_moment(mu03, 3, 0, m00),
		calc_normalized_moment(mu21, 2, 1, m00),
		calc_normalized_moment(mu12, 1, 2, m00),
		calc_normalized_moment(mu03, 0, 3, m00),
	};
}

HuMoments calc_hu_moments(const NormalizedMoments& normalized_moments) noexcept
{
	const auto [nu20, nu11, nu02, nu30, nu21, nu12, nu03] = normalized_moments;

	return HuMoments {
		nu20 + nu02,
		sqr(nu20 - nu02) + 4*sqr(nu11),
		sqr(nu30 - 3*nu12) + sqr(3*nu21 - nu03),
		sqr(nu30 + nu12) + sqr(nu21 + nu03),
		(nu30 - 3*nu12)*(nu30 + nu12)*(sqr(nu30 + nu12) - 3*sqr(nu21 + nu03))
			+ (3*nu21 - nu03)*(nu21 + nu03)*(3*sqr(nu30 + nu12) - sqr(nu21 + nu03)),
		(nu20 - nu02)*(sqr(nu30 + nu12) - sqr(nu21 + nu03))
			+ 4*nu11*(nu30 + nu12)*(nu21 + nu03),
		(3*nu21 - nu03)*(nu21 + nu03)*(3*sqr(nu30 + nu12) - sqr(nu21 + nu03))
			- (nu30 - 3*nu12)*(nu21 + nu03)*(3*sqr(nu30 + nu12) - sqr(nu21 + nu03))
	};
}

HuMoments calc_blob_hu_moments(const Blob& blob) noexcept
{
	const auto spatial_moments = calc_spatial_moments(blob);
	const auto centroid = calc_centroid(spatial_moments);
	const auto central_moments = calc_central_moments(blob, centroid);
	const auto normalized_moments = calc_normalized_moments(central_moments, spatial_moments.m00);

	return calc_hu_moments(normalized_moments);
}

HuMomentsArray calc_blobs_hu_moments(const Blobs& blobs)
{
    auto hu_moments_array = HuMomentsArray();
    hu_moments_array.reserve(blobs.size());

    std::transform(blobs.begin(), blobs.end(),
    			   std::back_inserter(hu_moments_array),
                   calc_blob_hu_moments);
    return hu_moments_array;
}
