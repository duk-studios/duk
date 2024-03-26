/// 21/02/2024
/// scene.cpp

#include <duk_objects/scene.h>

namespace duk::scene {

Objects& Scene::objects() {
    return m_objects;
}

const Objects& Scene::objects() const {
    return m_objects;
}

Systems& Scene::systems() {
    return m_systems;
}

const Systems& Scene::systems() const {
    return m_systems;
}

void Scene::enter(Environment* environment) {
    m_systems.enter(m_objects, environment);
}

void Scene::update(Environment* environment) {
    m_systems.update(m_objects, environment);
}

void Scene::exit(Environment* environment) {
    m_systems.exit(m_objects, environment);
}

}// namespace duk::scene