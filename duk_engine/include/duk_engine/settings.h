/// 22/02/2024
/// project.h

#ifndef DUK_ENGINE_SETTINGS_H
#define DUK_ENGINE_SETTINGS_H

#include <duk_resource/resource.h>
#include <glm/vec2.hpp>
#include <string>

namespace duk::engine {

struct Settings {
    std::string name;
    duk::resource::Id scene;
    glm::ivec2 resolution;
};

Settings load_settings(const std::string& path);

}// namespace duk::engine

namespace duk::json {

template<>
inline void from_json<duk::engine::Settings>(const rapidjson::Value& jsonObject, duk::engine::Settings& settings) {
    from_json_member(jsonObject, "name", settings.name);
    from_json_member(jsonObject, "scene", settings.scene);
    from_json_member(jsonObject, "resolution", settings.resolution);
}

}// namespace duk::json

#endif// DUK_ENGINE_SETTINGS_H
