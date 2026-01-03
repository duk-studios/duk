/// 22/02/2024
/// project.h

#ifndef DUK_ENGINE_SETTINGS_H
#define DUK_ENGINE_SETTINGS_H

#include <duk_resource/resources.h>
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

namespace duk::type {

template<>
struct Type<duk::engine::Settings> : Class<duk::engine::Settings,
    Member<"name", &duk::engine::Settings::name>,
    Member<"scene", &duk::engine::Settings::scene>,
    Member<"resolution", &duk::engine::Settings::resolution>,
    Member<"resourceExtensions", &duk::engine::Settings::resourceExtensions>> {
};

}// namespace duk::type

#endif// DUK_ENGINE_SETTINGS_H
