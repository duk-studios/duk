/// 21/02/2024
/// scene.cpp

#include <duk_engine/scene/scene.h>

namespace duk::engine {

Scene::Scene() {
    m_systems.attach(&m_dispatcher);
}

duk::objects::Objects& Scene::objects() {
    return m_objects;
}

const duk::objects::Objects& Scene::objects() const {
    return m_objects;
}

duk::system::Systems& Scene::systems() {
    return m_systems;
}

const duk::system::Systems& Scene::systems() const {
    return m_systems;
}

void Scene::enter(duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    m_systems.enter(m_objects, globals, disabledGroupsMask);
}

void Scene::update(duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    m_objects.update(m_dispatcher, globals);
    m_systems.update(m_objects, globals, disabledGroupsMask);
}

void Scene::exit(duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    m_systems.exit(m_objects, globals, disabledGroupsMask);
}

}// namespace duk::engine