/// 21/02/2024
/// scene.cpp

#include <duk_engine/scene/scene.h>

#include <duk_objects/object_solver.h>

namespace duk::engine {

Scene::Scene()
    : m_systemDispatcher(m_componentDispatcher) {
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

void Scene::attach(duk::tools::Globals& globals) {
    m_systems.attach(globals, m_objects, m_systemDispatcher);
    duk::objects::ObjectSolver objectSolver(m_objects);
    objectSolver.solve(m_systems);
}

void Scene::enter(uint32_t disabledGroupsMask) {
    m_objects.update(m_componentDispatcher);
    m_systems.enter(disabledGroupsMask);
}

void Scene::update(uint32_t disabledGroupsMask) {
    m_objects.update(m_componentDispatcher);
    m_systems.update(disabledGroupsMask);
}

void Scene::exit(uint32_t disabledGroupsMask) {
    m_systems.exit(disabledGroupsMask);
}

}// namespace duk::engine