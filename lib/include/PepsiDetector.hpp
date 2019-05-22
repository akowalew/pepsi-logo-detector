#pragma once

#include "LogoDetector.hpp"

#include <array>
#include <memory>

#include <nlohmann/json.hpp>

#include "moments.hpp"

class PepsiDetector
	:	public LogoDetector
{
public:
	struct Config
	{
        Config();

        static Config from_json(const nlohmann::json& json);

		ColorRange blue_range;
		BlobAreaRange blue_blob_area_range;
		HuMomentRange blue_blob_hu0_range;
		HuMomentRange blue_blob_hu1_range;

		ColorRange red_range;
		BlobAreaRange red_blob_area_range;
		HuMomentRange red_blob_hu0_range;
		HuMomentRange red_blob_hu1_range;

		double max_blobs_centers_distance;
	};

	explicit PepsiDetector(const Config& config = Config());
	~PepsiDetector() override;

	PepsiDetector(const PepsiDetector& other);
	PepsiDetector& operator=(const PepsiDetector& other);

	PepsiDetector(PepsiDetector&& other);
	PepsiDetector& operator=(PepsiDetector&& other);

	Logos find_logos(const cv::Mat& img) const override;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};
