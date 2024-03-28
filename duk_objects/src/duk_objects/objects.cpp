/// 06/07/2023
/// objects.cpp

#include <duk_objects/objects.h>

namespace duk::objects {

ComponentRegistry g_componentRegistry;

ComponentRegistry* ComponentRegistry::instance() {
    return &g_componentRegistry;
}

const std::string& ComponentRegistry::name_of(uint32_t index) const {
    DUK_ASSERT(m_componentEntries[index] != nullptr);
    return m_componentEntries[index]->name();
}

uint32_t ComponentRegistry::index_of(const std::string& componentTypeName) const {
    return m_componentNameToIndex.at(componentTypeName);
}

Object::Id::Id()
    : Id(detail::kMaxObjects, 0) {
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
    : Object(detail::kMaxObjects, 0, nullptr) {
}

Object::Object(uint32_t index, uint32_t version, Objects* objects)
    : m_id(index, version)
    , m_objects(objects) {
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

Objects* Object::objects() const {
    return m_objects;
}

void Object::destroy() const {
    m_objects->destroy_object(m_id);
}

const ComponentMask& Object::component_mask() const {
    return m_objects->component_mask(m_id);
}

Objects::~Objects() {
    for (auto object: all()) {
        object.destroy();
    }
    update();
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
    if (std::find(m_destroyedIds.begin(), m_destroyedIds.end(), id) != m_destroyedIds.end()) {
        return;
    }
    m_destroyedIds.emplace_back(id);
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

void Objects::update() {
    if (!m_destroyedIds.empty()) {
        for (auto& id: m_destroyedIds) {
            auto index = id.index();
            m_versions[index]++;
            m_freeList.push_back(index);
            auto componentMask = m_componentMasks[index];
            for (auto componentIndex: componentMask.bits<true>()) {
                remove_component(index, componentIndex);
            }
        }
        std::sort(m_freeList.begin(), m_freeList.end());
        m_destroyedIds.clear();
    }
}

const ComponentMask& Objects::component_mask(const Object::Id& id) const {
    assert(valid_object(id));
    return m_componentMasks.at(id.index());
}

Objects::ObjectView::Iterator::Iterator(uint32_t i, Objects* objects, ComponentMask componentMask)
    : m_i(i)
    , m_freeListCursor(0)
    , m_objects(objects)
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

Objects::ObjectView::Iterator Objects::ObjectView::Iterator::operator+(int value) const {
    return Iterator(m_i + value, m_objects, m_componentMask);
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

Objects::ObjectView::ObjectView(Objects* objects, ComponentMask componentMask)
    : m_objects(objects)
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

}// namespace duk::objects
