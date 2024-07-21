/// 06/07/2023
/// objects.cpp

#include <duk_objects/objects.h>
#include <duk_objects/events.h>
#include <duk_objects/object_solver.h>
#include <ranges>

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

Objects::Objects()
    : m_dirty(false) {
}

Objects::~Objects() {
    for (auto object: all(true)) {
        object.destroy();
    }

    for (int index = 0; index < m_exitComponentMasks.size(); index++) {
        auto& componentMask = m_exitComponentMasks[index];
        for (auto componentIndex: componentMask.bits<true>()) {
            m_componentPools[componentIndex]->destruct(index);
            componentMask.reset(componentIndex);
        }
    }
}

ObjectHandle<false> Objects::add_object() {
    m_dirty = true;
    if (!m_freeList.empty()) {
        auto freeIndex = m_freeList.back();
        m_freeList.pop_back();
        auto version = m_versions[freeIndex];
        m_activeComponentMasks[freeIndex].reset();
        m_enterComponentMasks[freeIndex].reset();
        m_exitComponentMasks[freeIndex].reset();
        m_enterIndices[freeIndex] = true;
        return {freeIndex, version, this};
    }
    auto index = (uint32_t)m_versions.size();
    m_versions.resize(m_versions.size() + 1);
    m_enterComponentMasks.resize(m_versions.size());
    m_activeComponentMasks.resize(m_versions.size());
    m_exitComponentMasks.resize(m_versions.size());
    m_enterIndices.resize(m_versions.size());
    m_exitIndices.resize(m_versions.size());
    m_versions[index] = 0;
    m_enterIndices[index] = true;
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
    std::unordered_map<Id, Id> runtimeIds;
    for (const auto srcObj: src.all(true)) {
        const auto dstObj = copy_object(srcObj);
        if (!root.valid()) {
            root = dstObj;
        }
        runtimeIds[srcObj.id()] = dstObj.id();
    }
    ObjectSolver solver(*this, runtimeIds);
    for (auto runtimeId: runtimeIds | std::views::values) {
        auto runtimeObj = object(runtimeId);
        auto componentMask = runtimeObj.component_mask();
        for (auto componentIndex: componentMask.bits<true>()) {
            ComponentRegistry::instance()->solve(&solver, runtimeObj, componentIndex);
        }
    }
    return root;
}

void Objects::destroy_object(const Id& id) {
    const auto index = id.index();
    if (m_exitIndices[index]) {
        return;
    }
    m_exitIndices[index] = true;
    m_enterIndices[index] = false;
    m_exitComponentMasks[index] = m_activeComponentMasks[index];
    m_enterComponentMasks[index].reset();
    m_dirty = true;
}

ObjectHandle<false> Objects::object(const Id& id) {
    return {id.index(), id.version(), this};
}

ObjectHandle<true> Objects::object(const Id& id) const {
    return {id.index(), id.version(), this};
}

bool Objects::valid_object(const Id& id) const {
    const auto index = id.index();
    return m_versions.size() > index && m_versions[index] == id.version();
}

Objects::ObjectView<false> Objects::all(bool includeInactive) {
    return ObjectView<false>(this, {}, includeInactive);
}

Objects::ObjectView<true> Objects::all(bool includeInactive) const {
    return ObjectView<true>(this, {}, includeInactive);
}

ComponentMask Objects::component_mask(const Id& id) const {
    DUK_ASSERT(valid_object(id));
    const auto index = id.index();
    return m_activeComponentMasks.at(index);
}

void Objects::update(ComponentEventDispatcher& dispatcher) {
    if (!m_dirty) {
        return;
    }
    m_dirty = false;

    // swap with clean data to make sure that we don't iterate over modified vectors
    std::vector<ComponentMask> enterComponentMasks(m_enterComponentMasks.size());
    std::vector<ComponentMask> exitComponentMasks(m_exitComponentMasks.size());
    std::vector<bool> enterIndices(m_enterIndices.size());
    std::vector<bool> exitIndices(m_exitIndices.size());

    std::swap(enterComponentMasks, m_enterComponentMasks);
    std::swap(exitComponentMasks, m_exitComponentMasks);
    std::swap(enterIndices, m_enterIndices);
    std::swap(exitIndices, m_exitIndices);

    // we need to while iterate, because new ids might be added during event dispatching
    for (auto index = 0; index < enterComponentMasks.size(); index++) {
        auto& enterComponentMask = enterComponentMasks[index];
        for (auto componentIndex: enterComponentMask.bits<true>()) {
            enterComponentMask.reset(componentIndex);
            dispatcher.emit_component<ComponentEnterEvent>(object(Id(index, m_versions[index])), componentIndex);
        }
    }
    for (auto index = 0; index < enterIndices.size(); index++) {
        if (enterIndices[index]) {
            enterIndices[index] = false;
            dispatcher.emit_object<ObjectEnterEvent>(object(Id(index, m_versions[index])));
        }
    }
    std::vector<uint32_t> destroyedIndices;
    for (auto index = 0; index < exitIndices.size(); index++) {
        if (exitIndices[index]) {
            dispatcher.emit_object<ObjectExitEvent>(object(Id(index, m_versions[index])));
            exitIndices[index] = false;
            destroyedIndices.emplace_back(index);
        }
    }
    for (auto index = 0; index < exitComponentMasks.size(); index++) {
        auto& exitComponentMask = exitComponentMasks[index];
        for (auto componentIndex: exitComponentMask.bits<true>()) {
            dispatcher.emit_component<ComponentExitEvent>(object(Id(index, m_versions[index])), componentIndex);
            exitComponentMask.reset(componentIndex);
            m_activeComponentMasks[index].reset(componentIndex);
            m_componentPools[componentIndex]->destruct(index);
        }
    }
    if (!destroyedIndices.empty()) {
        for (auto index: destroyedIndices) {
            m_versions[index]++;
            m_freeList.push_back(index);
        }
        std::ranges::sort(m_freeList);
    }
}

void Objects::add_component(uint32_t index, uint32_t componentIndex) {
    m_enterComponentMasks[index].set(componentIndex);
    m_activeComponentMasks[index].set(componentIndex);
    auto& pool = m_componentPools[componentIndex];
    if (!pool) {
        pool = ComponentRegistry::instance()->create_pool(componentIndex);
    }
    pool->construct(index);
    m_dirty = true;
}

void Objects::remove_component(uint32_t index, uint32_t componentIndex) {
    m_exitComponentMasks[index].set(componentIndex);
    m_dirty = true;
}

void Objects::solve_references() {
    ObjectSolver solver(*this);
    for (auto object: all(true)) {
        auto componentMask = object.component_mask();
        for (auto componentIndex: componentMask.bits<true>()) {
            ComponentRegistry::instance()->solve(&solver, object, componentIndex);
        }
    }
}

ComponentEventListener::ComponentEventListener()
    : m_dispatcher(nullptr) {
}

void ComponentEventListener::attach(ComponentEventDispatcher* dispatcher) {
    if (m_dispatcher == dispatcher) {
        return;
    }
    m_listener.clear();
    m_dispatcher = dispatcher;
}

}// namespace duk::objects
