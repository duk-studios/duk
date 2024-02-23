/// 21/02/2024
/// scene.cpp

#include <duk_scene/scene.h>

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

}// namespace duk::scene