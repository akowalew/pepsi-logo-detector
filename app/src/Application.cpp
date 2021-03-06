#include "Application.hpp"

#include <cstdio>

#include <fstream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "PepsiDetector.hpp"

namespace fs = std::filesystem;

namespace {

cv::Mat read_image(const std::string& src_file, int flags)
{
	assert(!src_file.empty());

	const auto img = cv::imread(src_file, flags);
	if(img.empty())
	{
		throw std::runtime_error("Image input file invalid");
	}

	return img;
}

void draw_logos(cv::Mat& dst, const Logos& logos)
{
	for(const auto& logo : logos)
	{
		cv::rectangle(dst, logo, cv::Scalar{0, 255, 0});
	}
}

void write_image(const std::string& dst_file, const cv::Mat& img)
{
	assert(!dst_file.empty());

	if(!cv::imwrite(dst_file, img))
	{
		throw std::runtime_error("Could not write image file");
	}
}

PepsiDetector::Config read_config(const std::string& config_file)
{
	spdlog::debug("Opening config file...");
	auto ifs = std::ifstream(config_file);
	if(!ifs)
	{
		throw std::runtime_error("Could not open configuration file");
	}

	spdlog::debug("Reading config into JSON object...");
	nlohmann::json json;
	if(!(ifs >> json))
	{
		throw std::runtime_error("Could not read configuration file");
	}

	spdlog::debug("Parsing config from JSON...");
	return PepsiDetector::Config::from_json(json);
}

std::string get_config_file_path(const std::string& src_file)
{
	const auto src_path = fs::path{src_file};
	const auto src_parent_path = src_path.parent_path();
	const auto config_file_path = (src_parent_path / "config.json");
	return config_file_path.string();
}

} // namespace

Application::Application(Options options)
	:	m_options(std::move(options))
{
	spdlog::debug("Initialized");
}

int Application::exec()
{
    spdlog::info("Reading input image...");
    const auto src_img = read_image(m_options.src_file, cv::IMREAD_COLOR);

	auto config_file = m_options.config_file;
	if(config_file.empty())
	{
		spdlog::info("Locating config file...");
		config_file = get_config_file_path(m_options.src_file);
	}

	const auto config = [this, &config_file]() {
		if(fs::exists(config_file) && fs::is_regular_file(config_file))
		{
			spdlog::info("Reading config file...");
			return read_config(config_file);
		}

		spdlog::info("Loading default config...");
		return PepsiDetector::Config{};
	}();

	spdlog::info("Creating pepsi detector...");
    auto detector = PepsiDetector{config};

    spdlog::info("Finding pepsi logos...");
    const auto logos = detector.find_logos(src_img);

    spdlog::info("Found {} pepsi logos:", logos.size());
    for(const auto& logo : logos)
    {
    	spdlog::info(" ({}, {}, {}, {})", logo.x, logo.y, logo.width, logo.height);
    }

    spdlog::info("Drawing logos on output image...");
    auto dst_img = src_img.clone();
    draw_logos(dst_img, logos);

    if(m_options.dst_file.empty())
    {
	    cv::imshow("Output image", dst_img);
	    cv::waitKey(0);
    }
    else
    {
	    spdlog::info("Writing output image...");
	    write_image(m_options.dst_file, dst_img);
    }

    spdlog::info("Finished");
    return 0;
}
