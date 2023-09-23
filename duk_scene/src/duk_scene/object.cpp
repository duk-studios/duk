/// 06/07/2023
/// object.cpp

#include <duk_scene/object.h>
#include <duk_scene/scene.h>

namespace duk::scene {


Object::Id::Id() : Id(MAX_OBJECTS, 0) {

}

Object::Id::Id(uint32_t index, uint32_t version) :
    m_index(index),
    m_version(version) {

}

uint32_t Object::Id::index() const {
    return m_index;
}

uint32_t Object::Id::version() const {
    return m_version;
}

bool Object::Id::operator==(const Object::Id& rhs) const {
    return m_index == rhs.m_index && m_version == rhs.m_version;
}

bool Object::Id::operator!=(const Object::Id& rhs) const {
    return !(*this == rhs);
}

Object::Object() : Object(MAX_OBJECTS, 0, nullptr) {

}

Object::Object(uint32_t index, uint32_t version, Scene* scene) :
    m_id(index, version),
    m_scene(scene) {

}

Object::Id Object::id() const {
    return m_id;
}

bool Object::valid() const {
    return m_scene != nullptr && m_scene->valid_object(m_id);
}

Object::operator bool() const {
    return valid();
}

}