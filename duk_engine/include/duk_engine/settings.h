/// 22/02/2024
/// project.h

#ifndef DUK_ENGINE_SETTINGS_H
#define DUK_ENGINE_SETTINGS_H

#include <duk_resource/set.h>
#include <glm/vec2.hpp>
#include <string>

namespace duk::engine {

struct Settings {
    std::string name;
    duk::resource::Id scene;
    duk::resource::LoadMode loadMode;
    glm::ivec2 resolution;
    std::vector<std::string> resourceExtensions;
};

Settings load_settings(const std::string& path);

}// namespace duk::engine

namespace duk::serial {

template<>
inline void from_json<duk::engine::Settings>(const rapidjson::Value& json, duk::engine::Settings& settings) {
    from_json_member(json, "name", settings.name);
    from_json_member(json, "scene", settings.scene);
    from_json_member(json, "resolution", settings.resolution);
    from_json_member(json, "resourceExtensions", settings.resourceExtensions, true);
}

template<>
inline void to_json<duk::engine::Settings>(rapidjson::Document& document, rapidjson::Value& json, const duk::engine::Settings& settings) {
    to_json_member(document, json, "name", settings.name);
    to_json_member(document, json, "scene", settings.scene);
    to_json_member(document, json, "resolution", settings.resolution);
    to_json_member(document, json, "resourceExtensions", settings.resourceExtensions);
}

}// namespace duk::serial

#endif// DUK_ENGINE_SETTINGS_H
