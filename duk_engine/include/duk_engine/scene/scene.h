/// 21/02/2024
/// scene.h

#ifndef DUK_ENGINE_SCENE_H
#define DUK_ENGINE_SCENE_H

#include <duk_system/system.h>
#include <duk_objects/objects.h>

namespace duk::engine {

class Scene {
public:
    Scene();

    DUK_NO_DISCARD duk::objects::Objects& objects();

    DUK_NO_DISCARD const duk::objects::Objects& objects() const;

    DUK_NO_DISCARD duk::system::Systems& systems();

    DUK_NO_DISCARD const duk::system::Systems& systems() const;

    void attach(duk::tools::Globals& globals);

    void enter(uint32_t disabledGroupsMask);

    void update(uint32_t disabledGroupsMask);

    void exit(uint32_t disabledGroupsMask);

    friend struct duk::type::Type<Scene>;

private:
    duk::objects::ComponentEventDispatcher m_componentDispatcher;
    duk::system::SystemEventDispatcher m_systemDispatcher;
    duk::objects::Objects m_objects;
    duk::system::Systems m_systems;
};

using SceneResource = duk::resource::Handle<Scene>;

}// namespace duk::engine

namespace duk::type {
// clang-format off
template<>
struct Type<duk::engine::Scene> : Class<duk::engine::Scene,
    Member<"objects", &duk::engine::Scene::m_objects>,
    Member<"systems", &duk::engine::Scene::m_systems>> {
};

// clang-format on
}// namespace duk::type

#endif// DUK_ENGINE_SCENE_H
