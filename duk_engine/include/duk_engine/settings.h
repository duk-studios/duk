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

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::engine::Settings& settings) {
    visitor->visit_member(settings.name, MemberDescription("name"));
    visitor->visit_member(settings.scene, MemberDescription("scene"));
    visitor->visit_member(settings.resolution, MemberDescription("resolution"));
}

}// namespace duk::serial

#endif// DUK_ENGINE_SETTINGS_H
