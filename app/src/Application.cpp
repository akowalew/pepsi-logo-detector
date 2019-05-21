#include "Application.hpp"

#include <cstdio>

#include <fstream>
#include <filesystem>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "PepsiDetector.hpp"

namespace fs = std::filesystem;

namespace {

class Logger
{
public:
	Logger(bool verbose)
		:	m_verbose(verbose)
	{}

	void operator()(const char* format, ...)
		__attribute__ ((format (printf, 2, 3)))
	{
		if(m_verbose)
		{
		    va_list arglist;
		    va_start(arglist, format);
		    vfprintf(stderr, format, arglist);
		    va_end(arglist);
		    putc('\n', stderr);
		}
	}

private:
	bool m_verbose {false};
};

cv::Mat read_image(const std::string& src_file, int flags)
{
	assert(!src_file.empty());

	auto img = cv::imread(src_file, flags);
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
		const auto color = cv::Scalar{0, 255, 0};
		const auto thickness = 1;
		cv::rectangle(dst, logo, color, thickness);
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
	auto ifs = std::ifstream(config_file);
	if(!ifs)
	{
		throw std::runtime_error("Could not open configuration file");
	}

	nlohmann::json json;
	if(!(ifs >> json))
	{
		throw std::runtime_error("Could not read configuration file");
	}

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

}

int Application::exec()
{
	auto logger = Logger{m_options.verbose};

    logger("Reading image...");
    const auto src_img = read_image(m_options.src_file, cv::IMREAD_COLOR);

	auto config_file = m_options.config_file;
	if(config_file.empty())
	{
		logger("Locating config file...");
		config_file = get_config_file_path(m_options.src_file);
	}

	const auto config = [&logger, &config_file]() {
		if(fs::exists(config_file) && fs::is_regular_file(config_file))
		{
			logger("Reading config file...");
			return read_config(config_file);
		}

		logger("Loading default config...");
		return PepsiDetector::Config{};
	}();

	logger("Creating pepsi detector...");
    auto detector = PepsiDetector{config};

    logger("Finding pepsi logos...");
    const auto logos = detector.find_logos(src_img);

    logger("Found %ld pepsi logos", logos.size());
    for(const auto& logo : logos)
    {
    	logger("\t(%d, %d, %d, %d)", logo.x, logo.y, logo.width, logo.height);
    }

    logger("Drawing logos on output image...");
    auto dst_img = src_img.clone();
    draw_logos(dst_img, logos);

    if(m_options.dst_file.empty())
    {
	    cv::imshow("Output image", dst_img);
	    cv::waitKey(0);
    }
    else
    {
	    logger("Writing output image...");
	    write_image(m_options.dst_file, dst_img);
    }

    logger("Finished");
    return 0;
}
