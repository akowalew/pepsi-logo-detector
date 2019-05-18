#pragma once

#include "LogoDetector.hpp"

#include <array>
#include <memory>

#include "moments.hpp"

class PepsiDetector
	:	public LogoDetector
{
public:
	struct Config
	{
        Config();

		ColorRange blue_range;
		BlobAreaRange blue_blob_area_range;
		HuMomentRange blue_blob_hu0_range;
		HuMomentRange blue_blob_hu1_range;

		std::array<ColorRange, 2> red_ranges;
		BlobAreaRange red_blob_area_range;
		HuMomentRange red_blob_hu0_range;
		HuMomentRange red_blob_hu1_range;
	};

	explicit PepsiDetector(const Config& config = Config());
	~PepsiDetector() override;

	PepsiDetector(const PepsiDetector& other);
	PepsiDetector& operator=(const PepsiDetector& other);

	PepsiDetector(PepsiDetector&& other);
	PepsiDetector& operator=(PepsiDetector&& other);

	Logos find_logos(const cv::Mat& img) const override;
	void train() override;

	const Config& config() const noexcept;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};
