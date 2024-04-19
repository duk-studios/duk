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

Id::Id()
    : Id(detail::kMaxObjects, 0) {
}

Id::Id(uint32_t index, uint32_t version)
    : m_index(index)
    , m_version(version) {
}

uint32_t Id::index() const {
    return m_index;
}

uint32_t Id::version() const {
    return m_version;
}

bool Id::operator==(const Id& rhs) const {
    return m_index == rhs.m_index && m_version == rhs.m_version;
}

bool Id::operator!=(const Id& rhs) const {
    return !(*this == rhs);
}

Objects::~Objects() {
    for (auto object: all()) {
        object.destroy();
    }
    update();
}

ObjectHandle<false> Objects::add_object() {
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

ObjectHandle<false> Objects::copy_object(const ObjectHandle<true>& src) {
    auto dst = add_object();
    auto originalComponentMask = src.component_mask();
    for (auto componentIndex: originalComponentMask.bits<true>()) {
        ComponentRegistry::instance()->copy_component(src, dst, componentIndex);
    }
    return dst;
}

ObjectHandle<false> Objects::copy_objects(const Objects& src) {
    ObjectHandle<false> root;
    for (const auto obj: src.all()) {
        const auto copy = copy_object(obj);
        if (!root.valid()) {
            root = copy;
        }
    }
    return root;
}

void Objects::destroy_object(const class Id& id) {
    if (std::ranges::find(m_destroyedIds, id) != m_destroyedIds.end()) {
        return;
    }
    m_destroyedIds.emplace_back(id);
}

ObjectHandle<false> Objects::object(const class Id& id) {
    return {id.index(), id.version(), this};
}

ObjectHandle<true> Objects::object(const class Id& id) const {
    return {id.index(), id.version(), this};
}

bool Objects::valid_object(const class Id& id) const {
    const auto index = id.index();
    return m_versions.size() > index && m_versions[index] == id.version();
}

Objects::ObjectView<false> Objects::all() {
    return ObjectView<false>(this, {});
}

Objects::ObjectView<true> Objects::all() const {
    return ObjectView<true>(this, {});
}

const ComponentMask& Objects::component_mask(const class Id& id) const {
    assert(valid_object(id));
    return m_componentMasks.at(id.index());
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
        std::ranges::sort(m_freeList);
        m_destroyedIds.clear();
    }
}

}// namespace duk::objects
