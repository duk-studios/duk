/// 21/02/2024
/// scene.cpp

#include <duk_engine/scene/scene.h>

namespace duk::engine {

duk::objects::Objects& Scene::objects() {
    return m_objects;
}

const duk::objects::Objects& Scene::objects() const {
    return m_objects;
}

Systems& Scene::systems() {
    return m_systems;
}

const Systems& Scene::systems() const {
    return m_systems;
}

void Scene::enter(Engine& engine) {
    m_systems.enter(m_objects, engine);
}

void Scene::update(Engine& engine) {
    m_systems.update(m_objects, engine);
}

void Scene::exit(Engine& engine) {
    m_systems.exit(m_objects, engine);
}

}// namespace duk::engine