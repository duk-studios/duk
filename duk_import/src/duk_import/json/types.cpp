//
// Created by rov on 12/5/2023.
//

#include <duk_import/json/types.h>
#include <sstream>

namespace duk::import::json {

duk::renderer::LightValue to_light_value(const rapidjson::Value& member) {
    duk::renderer::LightValue lightValue = {};
    lightValue.color = to_vec3(member["color"]);
    lightValue.intensity = member["intensity"].GetFloat();
    return lightValue;
}


}