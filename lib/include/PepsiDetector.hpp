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
		ColorRange left_red_range;
		ColorRange right_red_range;
		BlobAreaRange blob_area_range;
		HuMomentRange hu0_range;
		HuMomentRange hu1_range;
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
