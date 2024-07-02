/// 06/07/2023
/// objects.cpp

#include <duk_objects/objects.h>
#include <duk_objects/events.h>

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
    : m_dispatcher(nullptr) {
}

Objects::~Objects() {
    for (auto object: all()) {
        object.destroy();
    }
    update_destroy();
}

ObjectHandle<false> Objects::add_object() {
    if (!m_freeList.empty()) {
        auto freeIndex = m_freeList.back();
        m_freeList.pop_back();
        auto version = m_versions[freeIndex];
        m_componentMasks[freeIndex].reset();
        auto id = m_idsToCreate.emplace_back(freeIndex, version);
        return {id, this};
    }
    auto index = (uint32_t)m_versions.size();
    m_versions.resize(m_versions.size() + 1);
    m_componentMasks.resize(m_versions.size());
    m_versions[index] = 0;
    auto id = m_idsToCreate.emplace_back(index, 0);
    return {id, this};
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

void Objects::destroy_object(const Id& id) {
    if (std::ranges::find(m_idsToDestroy, id) != m_idsToDestroy.end()) {
        return;
    }
    m_idsToDestroy.emplace_back(id);
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

Objects::ObjectView<false> Objects::all() {
    return ObjectView<false>(this, {});
}

Objects::ObjectView<true> Objects::all() const {
    return ObjectView<true>(this, {});
}

const ComponentMask& Objects::component_mask(const Id& id) const {
    assert(valid_object(id));
    return m_componentMasks.at(id.index());
}

void Objects::attach_dispatcher(ComponentEventDispatcher* dispatcher) {
    m_dispatcher = dispatcher;
}

void Objects::update() {
    update_create();
    update_destroy();
}

void Objects::add_component(uint32_t index, uint32_t componentIndex) {
    m_componentMasks[index].set(componentIndex);
    auto& pool = m_componentPools[componentIndex];
    if (!pool) {
        pool = ComponentRegistry::instance()->create_pool(componentIndex);
    }
    pool->construct(index);
    if (m_dispatcher) {
        m_dispatcher->emit_one<ComponentEnterEvent>(object(Id(index, m_versions[index])), componentIndex);
    }
}

void Objects::remove_component(uint32_t index, uint32_t componentIndex) {
    if (m_dispatcher) {
        m_dispatcher->emit_one<ComponentExitEvent>(object(Id(index, m_versions[index])), componentIndex);
    }
    m_componentPools[componentIndex]->destruct(index);
    m_componentMasks[index].reset(componentIndex);
}

void Objects::update_create() {
    // we need to while iterate, because new ids might be added during event dispatching
    while (!m_idsToCreate.empty()) {
        std::vector<Id> idsToCreate;
        std::swap(idsToCreate, m_idsToCreate);
        if (m_dispatcher) {
            for (auto& id: idsToCreate) {
                m_dispatcher->emit_all<ObjectEnterEvent>(object(id));
            }
        }
    }
}

void Objects::update_destroy() {
    // same as create, we need to while iterate, because ids might be added during event dispatching
    while (!m_idsToDestroy.empty()) {
        std::vector<Id> idsToDestroy;
        std::swap(idsToDestroy, m_idsToDestroy);
        for (auto& id: idsToDestroy) {
            if (m_dispatcher) {
                m_dispatcher->emit_all<ObjectExitEvent>(object(id));
            }

            auto index = id.index();
            m_versions[index]++;
            m_freeList.push_back(index);
            auto componentMask = m_componentMasks[index];
            for (auto componentIndex: componentMask.bits<true>()) {
                remove_component(index, componentIndex);
            }
        }
        std::ranges::sort(m_freeList);
    }
}
}// namespace duk::objects
