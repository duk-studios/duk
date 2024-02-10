/// 06/07/2023
/// scene.cpp

#include <duk_scene/scene.h>

namespace duk::scene {

uint32_t ComponentRegistry::s_componentIndexCounter = 0;

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

Scene* Object::scene() const {
    return m_scene;
}

void Object::destroy() const {
    m_scene->destroy_object(m_id);
}

Object::ComponentView Object::components() {
    return m_scene->components(m_id);
}

Object::ComponentIterator::ComponentIterator(const ComponentMask& mask, uint32_t index) :
        m_mask(mask), m_i(index) {
    next();
}

bool Object::ComponentIterator::operator==(const Object::ComponentIterator& rhs) {
    return (&m_mask) == (&rhs.m_mask) && m_i == rhs.m_i;
}

bool Object::ComponentIterator::operator!=(const Object::ComponentIterator& rhs) {
    return !(*this == rhs);
}

Object::ComponentIterator Object::ComponentIterator::operator++() {
    ++m_i;
    next();
    return *this;
}

uint32_t Object::ComponentIterator::operator*() {
    return m_i;
}

void Object::ComponentIterator::next() {
    if (m_i >= MAX_COMPONENTS) {
        return;
    }
    m_i += m_mask.countr_zero(m_i);
}

Object::ComponentView::ComponentView(const ComponentMask& mask) :
        m_mask(mask) {

}

Object::ComponentIterator Object::ComponentView::begin() {
    return Object::ComponentIterator(m_mask, 0);
}

Object::ComponentIterator Object::ComponentView::end() {
    return Object::ComponentIterator(m_mask, MAX_COMPONENTS);
}


Scene::~Scene() {
    for (auto object : objects()) {
        object.destroy();
    }
}

Object Scene::add_object() {
    if (!m_freeList.empty()) {
        auto freeIndex = m_freeList.back();
        m_freeList.pop_back();
        auto version = m_objectVersions[freeIndex];
        m_objectComponentMasks[freeIndex].reset();
        return {freeIndex, version, this};
    }
    auto index = (uint32_t)m_objectVersions.size();
    m_objectVersions.resize(m_objectVersions.size() + 1);
    m_objectComponentMasks.resize(m_objectVersions.size());
    m_objectVersions[index] = 0;
    return {index, 0, this};
}

void Scene::destroy_object(const Object::Id& id) {
    assert(valid_object(id));
    auto index = id.index();
    m_objectVersions[index]++;
    m_freeList.push_back(index);
    std::sort(m_freeList.begin(), m_freeList.end());

    auto componentMask = m_objectComponentMasks[index];
    duk::tools::for_each_bit<true>(componentMask, [index, this](size_t componentIndex) {
        remove_component(index, componentIndex);
    });
}

Object Scene::object(const Object::Id& id) {
    return {id.index(), id.version(), this};
}

bool Scene::valid_object(const Object::Id& id) const {
    const auto index = id.index();
    return m_objectVersions.size() > index && m_objectVersions[index] == id.version();
}

Scene::ObjectView Scene::objects() {
    return {this, {}};
}

void Scene::remove_component(uint32_t index, uint32_t componentIndex) {
    m_componentPools[componentIndex]->destruct(index);
    m_objectComponentMasks[index].reset(componentIndex);
}

Object::ComponentView Scene::components(const Object::Id& id) {
    assert(valid_object(id));
    return Object::ComponentView(m_objectComponentMasks.at(id.index()));
}

Scene::ObjectView::Iterator::Iterator(uint32_t i, Scene* scene, ComponentMask componentMask) :
    m_i(i),
    m_freeListCursor(0),
    m_scene(scene),
    m_componentMask(componentMask) {
    next();
}

Object Scene::ObjectView::Iterator::operator*() const {
    return m_scene->object(Object::Id{m_i, m_scene->m_objectVersions[m_i]});
}

Scene::ObjectView::Iterator& Scene::ObjectView::Iterator::operator++() {
    m_i++;
    next();
    return *this;
}

Scene::ObjectView::Iterator Scene::ObjectView::Iterator::operator++(int) {
    auto old = *this;
    m_i++;
    next();
    return old;
}

bool Scene::ObjectView::Iterator::operator==(const Scene::ObjectView::Iterator& other) const {
    return m_i == other.m_i && m_scene == other.m_scene;
}

bool Scene::ObjectView::Iterator::operator!=(const Scene::ObjectView::Iterator& other) const {
    return !(*this == other);
}

void Scene::ObjectView::Iterator::next() {
    while (m_i < m_scene->m_objectVersions.size() && !valid_object()) {
        m_i++;
        m_freeListCursor++;
    }
}

bool Scene::ObjectView::Iterator::valid_object() {
    if ((m_componentMask & m_scene->m_objectComponentMasks[m_i]) != m_componentMask) {
        return false;
    }
    auto& freeList = m_scene->m_freeList;
    if (freeList.empty() || m_freeListCursor >= freeList.size()) {
        return true;
    }

    return freeList[m_freeListCursor] != m_i;
}

Scene::ObjectView::ObjectView(Scene* scene, ComponentMask componentMask) :
    m_scene(scene),
    m_componentMask(componentMask) {

}

Scene::ObjectView::Iterator Scene::ObjectView::begin() {
    return {0, m_scene, m_componentMask};
}

Scene::ObjectView::Iterator Scene::ObjectView::begin() const {
    return {0, m_scene, m_componentMask};
}

Scene::ObjectView::Iterator Scene::ObjectView::end() {
    return {static_cast<uint32_t>(m_scene->m_objectVersions.size()), m_scene, m_componentMask};
}

Scene::ObjectView::Iterator Scene::ObjectView::end() const {
    return {static_cast<uint32_t>(m_scene->m_objectVersions.size()), m_scene, m_componentMask};
}

}
