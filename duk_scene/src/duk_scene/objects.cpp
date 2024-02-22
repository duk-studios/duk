/// 06/07/2023
/// scene.cpp

#include <duk_scene/objects.h>

namespace duk::scene {

uint32_t ComponentRegistry::s_componentIndexCounter = 0;

Object::Id::Id()
    : Id(MAX_OBJECTS, 0) {
}

Object::Id::Id(uint32_t index, uint32_t version)
    : m_index(index)
    , m_version(version) {
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

Object::Object()
    : Object(MAX_OBJECTS, 0, nullptr) {
}

Object::Object(uint32_t index, uint32_t version, Objects* scene)
    : m_id(index, version)
    , m_objects(scene) {
}

Object::Id Object::id() const {
    return m_id;
}

bool Object::valid() const {
    return m_objects != nullptr && m_objects->valid_object(m_id);
}

Object::operator bool() const {
    return valid();
}

Objects* Object::scene() const {
    return m_objects;
}

void Object::destroy() const {
    m_objects->destroy_object(m_id);
}

Object::ComponentView Object::components() {
    return m_objects->components(m_id);
}

Object::ComponentIterator::ComponentIterator(const ComponentMask& mask, uint32_t index)
    : m_mask(mask)
    , m_i(index) {
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

Object::ComponentView::ComponentView(const ComponentMask& mask)
    : m_mask(mask) {
}

Object::ComponentIterator Object::ComponentView::begin() {
    return Object::ComponentIterator(m_mask, 0);
}

Object::ComponentIterator Object::ComponentView::end() {
    return Object::ComponentIterator(m_mask, MAX_COMPONENTS);
}

Objects::~Objects() {
    for (auto object: all()) {
        object.destroy();
    }
}

Object Objects::add_object() {
    if (!m_freeList.empty()) {
        auto freeIndex = m_freeList.back();
        m_freeList.pop_back();
        auto version = m_versions[freeIndex];
        m_componentMasks[freeIndex].reset();
        return {freeIndex, version, this};
    }
    auto index = (uint32_t)m_versions.size();
    m_versions.resize(m_versions.size() + 1);
    m_componentMasks.resize(m_versions.size());
    m_versions[index] = 0;
    return {index, 0, this};
}

void Objects::destroy_object(const Object::Id& id) {
    assert(valid_object(id));
    auto index = id.index();
    m_versions[index]++;
    m_freeList.push_back(index);
    std::sort(m_freeList.begin(), m_freeList.end());

    auto componentMask = m_componentMasks[index];
    duk::tools::for_each_bit<true>(componentMask, [index, this](size_t componentIndex) {
        remove_component(index, componentIndex);
    });
}

Object Objects::object(const Object::Id& id) {
    return {id.index(), id.version(), this};
}

bool Objects::valid_object(const Object::Id& id) const {
    const auto index = id.index();
    return m_versions.size() > index && m_versions[index] == id.version();
}

Objects::ObjectView Objects::all() {
    return {this, {}};
}

void Objects::remove_component(uint32_t index, uint32_t componentIndex) {
    m_componentPools[componentIndex]->destruct(index);
    m_componentMasks[index].reset(componentIndex);
}

Object::ComponentView Objects::components(const Object::Id& id) {
    assert(valid_object(id));
    return Object::ComponentView(m_componentMasks.at(id.index()));
}

Objects::ObjectView::Iterator::Iterator(uint32_t i, Objects* scene, ComponentMask componentMask)
    : m_i(i)
    , m_freeListCursor(0)
    , m_objects(scene)
    , m_componentMask(componentMask) {
    next();
}

Object Objects::ObjectView::Iterator::operator*() const {
    return m_objects->object(Object::Id{m_i, m_objects->m_versions[m_i]});
}

Objects::ObjectView::Iterator& Objects::ObjectView::Iterator::operator++() {
    m_i++;
    next();
    return *this;
}

Objects::ObjectView::Iterator Objects::ObjectView::Iterator::operator++(int) {
    auto old = *this;
    m_i++;
    next();
    return old;
}

bool Objects::ObjectView::Iterator::operator==(const Objects::ObjectView::Iterator& other) const {
    return m_i == other.m_i && m_objects == other.m_objects;
}

bool Objects::ObjectView::Iterator::operator!=(const Objects::ObjectView::Iterator& other) const {
    return !(*this == other);
}

void Objects::ObjectView::Iterator::next() {
    while (m_i < m_objects->m_versions.size() && !valid_object()) {
        m_i++;
        m_freeListCursor++;
    }
}

bool Objects::ObjectView::Iterator::valid_object() {
    if ((m_componentMask & m_objects->m_componentMasks[m_i]) != m_componentMask) {
        return false;
    }
    auto& freeList = m_objects->m_freeList;
    if (freeList.empty() || m_freeListCursor >= freeList.size()) {
        return true;
    }

    return freeList[m_freeListCursor] != m_i;
}

Objects::ObjectView::ObjectView(Objects* scene, ComponentMask componentMask)
    : m_objects(scene)
    , m_componentMask(componentMask) {
}

Objects::ObjectView::Iterator Objects::ObjectView::begin() {
    return {0, m_objects, m_componentMask};
}

Objects::ObjectView::Iterator Objects::ObjectView::begin() const {
    return {0, m_objects, m_componentMask};
}

Objects::ObjectView::Iterator Objects::ObjectView::end() {
    return {static_cast<uint32_t>(m_objects->m_versions.size()), m_objects, m_componentMask};
}

Objects::ObjectView::Iterator Objects::ObjectView::end() const {
    return {static_cast<uint32_t>(m_objects->m_versions.size()), m_objects, m_componentMask};
}

}// namespace duk::scene
