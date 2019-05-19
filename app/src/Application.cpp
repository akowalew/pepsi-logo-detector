#include "Application.hpp"

#include <cstdio>
#include <unistd.h>

#include <sstream>

#include <opencv2/opencv.hpp>

#include "PepsiDetector.hpp"

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

cv::Mat read_image(const std::string& ifile, int flags)
{
	const auto ifile_provided = (!ifile.empty());
	if(ifile_provided)
	{
		auto img = cv::imread(ifile, flags);
		if(img.empty())
		{
			throw std::runtime_error("Image input file invalid");
		}

		return img;
	}

	// Very hacky
	std::stringstream stdin_file;
	stdin_file << "/proc/" << getpid() << "/fd/0";

	auto img = cv::imread(stdin_file.str(), flags);
	if(img.empty())
	{
		throw std::runtime_error("Input image invalid");
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

void write_image(const std::string& ofile, const cv::Mat& img)
{
	const auto ofile_provided = (!ofile.empty());
	if(ofile_provided)
	{
		if(!cv::imwrite(ofile, img))
		{
			throw std::runtime_error("Could not write image file");
		}

		return;
	}

	// Very hacky
	std::stringstream stdout_file;
	stdout_file << "/proc/" << getpid() << "/fd/1";

	if(!cv::imwrite(stdout_file.str(), img))
	{
		throw std::runtime_error("Could not write image");
	}
}

} // namespace

Application::Application(Options options)
	:	m_options(std::move(options))
{

}

int Application::exec()
{
	auto logger = Logger{m_options.verbose};

	logger("Creating pepsi detector...");
    auto detector = PepsiDetector{};

    logger("Reading image...");
    const auto src_img = read_image(m_options.ifile, cv::IMREAD_COLOR);

    logger("Finding pepsi logos...");
    const auto logos = detector.find_logos(src_img);

    logger("Found %ld pepsi logos", logos.size());
    for(const auto& logo : logos)
    {
    	logger("\t(%d, %d, %d, %d)", logo.x, logo.y, logo.width, logo.height);
    }

    logger("Creating output image...");
    auto dst_img = src_img.clone();

    logger("Drawing logos...");
    draw_logos(dst_img, logos);

    if(m_options.verbose)
    {
	    cv::imshow("Output image", dst_img);
    }

    logger("Writing output image...");
    write_image(m_options.ofile, dst_img);

    if(m_options.verbose)
    {
	    cv::waitKey(0);
    }

    logger("Finished");
    return 0;
}
