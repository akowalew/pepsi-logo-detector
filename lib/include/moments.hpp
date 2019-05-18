#pragma once

#include <array>
#include <vector>

#include <opencv2/opencv.hpp>

#include "blobs.hpp"
#include "types.hpp"

using SpatialMoment = long long;

using CentralMoment = long long;

using NormalizedMoment = double;

using HuMoment = double;

using Centroid = cv::Point2d;

struct SpatialMoments
{
	SpatialMoment m00, m10, m01, m20, m11, m02, m30, m21, m12, m03;
};

struct CentralMoments
{
	CentralMoment mu20, mu11, mu02, mu30, mu21, mu12, mu03;
};

struct NormalizedMoments
{
	NormalizedMoment nu20, nu11, nu02, nu30, nu21, nu12, nu03;
};

constexpr static auto HuMomentsMax = 7;

using HuMoments = std::array<HuMoment, HuMomentsMax>;

using HuMomentsArray = std::vector<HuMoments>;

using HuMomentRange = ValueRange<HuMoment>;

struct Moments
{
	SpatialMoments spatial;
	CentralMoments central;
	NormalizedMoments normalized;
	HuMoments hu;
	Centroid centroid;
};

SpatialMoments calc_spatial_moments(const Blob& blob) noexcept;

Centroid calc_centroid(const SpatialMoments& spatial_moments) noexcept;

CentralMoments calc_central_moments(const Blob& blob, Centroid centroid) noexcept;

NormalizedMoments calc_normalized_moments(const CentralMoments& central_moments,
										  SpatialMoment m00) noexcept;

HuMoments calc_hu_moments(const NormalizedMoments& normalized_moments) noexcept;

HuMoments calc_blob_hu_moments(const Blob& blob) noexcept;

HuMomentsArray calc_blobs_hu_moments(const Blobs& blobs);
