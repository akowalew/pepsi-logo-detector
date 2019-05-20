#include "PepsiDetector.hpp"

/**
 * @brief Default constructor for PepsiDetector's Config. Provides default values for params
 */
PepsiDetector::Config::Config()
    :   blue_range{{{100, 75, 0}}, {{130, 255, 255}}}
    ,   blue_blob_area_range{200, 2500}
    ,   blue_blob_hu0_range{0.30, 0.45}
    ,   blue_blob_hu1_range{0.05, 0.15}

    ,   red_range{{{165, 75, 75}}, {{10, 255, 255}}}
    ,   red_blob_area_range{500, 3000}
    ,	red_blob_hu0_range{0.18, 0.20}
	,	red_blob_hu1_range{0.006, 0.015}

    ,   max_blobs_centers_distance{30.0}
{}

PepsiDetector::Config PepsiDetector::Config::from_json(const nlohmann::json& json)
{
    Config config;

    const auto blue_range_object = json.at("blue_range");
    config.blue_range.min = blue_range_object.at("min");
    config.blue_range.max = blue_range_object.at("max");

    const auto blue_blob_area_range_object = json.at("blue_blob_area_range");
    config.blue_blob_area_range.min = blue_blob_area_range_object.at("min");
    config.blue_blob_area_range.max = blue_blob_area_range_object.at("max");

    const auto blue_blob_hu0_range_object = json.at("blue_blob_hu0_range");
    config.blue_blob_hu0_range.min = blue_blob_hu0_range_object.at("min");
    config.blue_blob_hu0_range.max = blue_blob_hu0_range_object.at("max");

    const auto blue_blob_hu1_range_object = json.at("blue_blob_hu1_range");
    config.blue_blob_hu1_range.min = blue_blob_hu1_range_object.at("min");
    config.blue_blob_hu1_range.max = blue_blob_hu1_range_object.at("max");

    const auto red_range_object = json.at("red_range");
    config.red_range.min = red_range_object.at("min");
    config.red_range.max = red_range_object.at("max");

    const auto red_blob_area_range_object = json.at("red_blob_area_range");
    config.red_blob_area_range.min = red_blob_area_range_object.at("min");
    config.red_blob_area_range.max = red_blob_area_range_object.at("max");

    const auto red_blob_hu0_range_object = json.at("red_blob_hu0_range");
    config.red_blob_hu0_range.min = red_blob_hu0_range_object.at("min");
    config.red_blob_hu0_range.max = red_blob_hu0_range_object.at("max");

    const auto red_blob_hu1_range_object = json.at("red_blob_hu1_range");
    config.red_blob_hu1_range.min = red_blob_hu1_range_object.at("min");
    config.red_blob_hu1_range.max = red_blob_hu1_range_object.at("max");

    config.max_blobs_centers_distance = json.at("max_blobs_centers_distance");

    return config;
}
