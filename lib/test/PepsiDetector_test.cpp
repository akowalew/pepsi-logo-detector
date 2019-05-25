#include <catch2/catch.hpp>

#include <fstream>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "PepsiDetector.hpp"

namespace {

using Image = cv::Mat;
using Images = std::vector<Image>;
using LogosArray = std::vector<Logos>;

Image read_image(const char* file_name)
{
    auto image = cv::imread(file_name, cv::IMREAD_COLOR);
    if(image.empty())
    {
        throw std::runtime_error("Could not read image file");
    }

    return image;
}

Images read_images(const std::vector<const char*>& files_names)
{
    auto images = std::vector<cv::Mat>();
    images.reserve(files_names.size());

    std::transform(files_names.begin(), files_names.end(),
                   std::back_inserter(images),
                   read_image);

    return images;
}

LogosArray find_logos_on_images(const Images& images, const PepsiDetector& detector)
{
    auto logos_array = std::vector<Logos>();
    logos_array.reserve(images.size());

    std::transform(images.begin(), images.end(),
                   std::back_inserter(logos_array),
                   [&detector](const auto& image)
                   {
                       return detector.find_logos(image);
                   });

    return logos_array;
}

bool logos_matching(const Logo& found_logo, const Logo& real_logo)
{
    const auto dx = (real_logo.x - found_logo.x);
    const auto dy = (real_logo.y - found_logo.y);
    const auto dwidth = (real_logo.width - found_logo.width);
    const auto dheight = (real_logo.height - found_logo.height);

    if(dx < 20 && dy < 20 && dwidth < 20 && dheight < 20)
    {
        return true;
    }

    return false;
}

PepsiDetector::Config read_config(const char* path)
{
    std::ifstream ifs(path);
    if(!ifs)
    {
        throw std::runtime_error("Could not open config JSON file");
    }

    nlohmann::json json;
    if(!(ifs >> json))
    {
        throw std::runtime_error("Could not read config JSON file");
    }

    return PepsiDetector::Config::from_json(json);
}

void debug_logos(const Logos& logos)
{
    for(const auto& logo : logos)
    {
        printf("(%d, %d, %d, %d) ", logo.x, logo.y, logo.width, logo.height);
    }

    putchar('\n');
}

} //

const auto IMAGES_FILES = std::vector<const char*>{
    "assets/camera/0.jpg",
    "assets/camera/1.jpg",
    "assets/camera/2.jpg",
    "assets/camera/3.jpg",
    "assets/camera/4.jpg",
    "assets/camera/5.jpg",
    "assets/camera/6.jpg",
    "assets/camera/7.jpg",
    "assets/camera/8.jpg",
    "assets/camera/9.jpg",
    "assets/camera/10.jpg",
};

const auto REAL_LOGOS_ARRAY = LogosArray {
    /* 0.jpg */ Logos{ Logo{200, 116, 74, 80}, Logo{315, 182, 56, 57}, Logo{424, 183, 57, 56} },
    /* 1.jpg */ Logos{ Logo{196, 288, 63, 52} },
    /* 2.jpg */ Logos{ Logo{262, 165, 70, 77} },
    /* 3.jpg */ Logos{ Logo{328, 182, 59, 58} },
    /* 4.jpg */ Logos{ Logo{419, 216, 50, 48}, Logo{303, 218, 48, 47}, Logo{558, 219, 52, 49} },
    /* 5.jpg */ Logos{ Logo{55, 95, 52, 54}, Logo{135, 140, 39, 40}, Logo{215, 142, 41, 39} },
    /* 6.jpg */ Logos{ Logo{402, 184, 56, 55} },
    /* 7.jpg */ Logos{ Logo{321, 258, 61, 58} },
    /* 8.jpg */ Logos{ Logo{512, 262, 56, 51} },
    /* 9.jpg */ Logos{ Logo{304, 65, 64, 61}, Logo{471, 66, 68, 61}, Logo{127, 68, 61, 58} },
    /* 10.jpg */ Logos{ Logo{245, 146, 60, 62}, Logo{426, 187, 45, 45}, Logo{347, 191, 42, 44} },
};

SCENARIO("Pepsi logos can be found on color image", "[PepsiDetector]")
{
    const auto config = read_config("assets/camera/config.json");
    const auto detector = PepsiDetector{config};

    GIVEN("Images from phone camera")
    {
        const auto images = read_images(IMAGES_FILES);

        WHEN("Finding logos")
        {
            const auto logos_array = find_logos_on_images(images, detector);
            REQUIRE(logos_array.size() == REAL_LOGOS_ARRAY.size());

            THEN("All pepsi logos should be found")
            {
                for(auto i = 0; i < logos_array.size(); ++i)
                {
                    const auto& logos = logos_array[i];
                    const auto& real_logos = REAL_LOGOS_ARRAY[i];

                    // printf("Logos: ");
                    // debug_logos(logos);
                    // printf("Real logos: ");
                    // debug_logos(real_logos);
                    REQUIRE(logos.size() == real_logos.size());

                    for(auto j = 0; j < logos.size(); ++j)
                    {
                        const auto& logo = logos[j];
                        const auto& real_logo = real_logos[j];
                        REQUIRE(logos_matching(logo, real_logo));
                    }
                }
            }
        }
    }
}
