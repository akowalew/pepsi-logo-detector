#include "PepsiDetector.hpp"

using nlohmann::json;

namespace cv {

void from_json(const json& j, Vec3b& v) {
    j.at(0).get_to(v[0]);
    j.at(1).get_to(v[1]);
    j.at(2).get_to(v[2]);
}

} // namespace cv

template<typename T>
void from_json(const json& j, ValueRange<T>& v) {
    j.at("min").get_to(v.min);
    j.at("max").get_to(v.max);
}

/**
 * @brief Default constructor for PepsiDetector's Config. Provides default values for params
 */
PepsiDetector::Config::Config()
    :   blue_range{{100, 75, 0}, {130, 255, 255}}
    ,   blue_blob_area_range{200, 2500}
    ,   blue_blob_hu0_range{0.30, 0.45}
    ,   blue_blob_hu1_range{0.05, 0.15}

    ,   red_range{{165, 75, 75}, {10, 255, 255}}
    ,   red_blob_area_range{500, 3000}
    ,	red_blob_hu0_range{0.18, 0.20}
	,	red_blob_hu1_range{0.006, 0.015}

    ,   max_blobs_centers_distance{30.0}
{}

PepsiDetector::Config PepsiDetector::Config::from_json(const nlohmann::json& json)
{
    Config config;
    config.blue_range = json.at("blue_range");
    config.blue_blob_area_range = json.at("blue_blob_area_range");
    config.blue_blob_hu0_range = json.at("blue_blob_hu0_range");
    config.blue_blob_hu1_range = json.at("blue_blob_hu1_range");
    config.red_range = json.at("red_range");
    config.red_blob_area_range = json.at("red_blob_area_range");
    config.red_blob_hu0_range = json.at("red_blob_hu0_range");
    config.red_blob_hu1_range = json.at("red_blob_hu1_range");
    config.max_blobs_centers_distance = json.at("max_blobs_centers_distance");

    return config;
}
