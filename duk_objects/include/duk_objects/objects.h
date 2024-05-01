/// 06/07/2023
/// objects.h

#ifndef DUK_OBJECTS_OBJECTS_H
#define DUK_OBJECTS_OBJECTS_H

#include <duk_log/log.h>

#include <duk_macros/assert.h>

#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <duk_serial/json/types.h>

#include <duk_tools/bit_block.h>
#include <duk_tools/fixed_vector.h>
#include <duk_tools/types.h>

#include <array>

namespace duk::objects {

namespace detail {

static constexpr uint32_t kComponentsPerChunk = 32;
static constexpr uint32_t kMaxComponents = 128;
static constexpr uint32_t kMaxObjects = std::numeric_limits<uint32_t>::max();

class ComponentPool {
public:
    virtual ~ComponentPool() = default;

    virtual void destruct(uint32_t index) = 0;
};

template<typename T>
class ComponentPoolT : public ComponentPool {
public:
    explicit ComponentPoolT(uint32_t componentsPerChunk)
        : m_componentsPerChunk(componentsPerChunk) {
    }

    ~ComponentPoolT() override {
        for (auto& chunk: m_chunks) {
            free(chunk);
        }
    }

    template<typename... Args>
    void construct(uint32_t index, Args&&... args) {
        auto ptr = get(index);
        ::new (ptr) T(std::forward<Args>(args)...);
    }

    void destruct(uint32_t index) override {
        auto ptr = get(index);
        ptr->~T();
    }

    DUK_NO_DISCARD T* get(uint32_t index) {
        const uint32_t chunkIndex = index / m_componentsPerChunk;
        const uint32_t indexInChunk = index % m_componentsPerChunk;
        auto chunk = get_chunk(chunkIndex);
        return chunk + indexInChunk;
    }

    DUK_NO_DISCARD const T* get(uint32_t index) const {
        const uint32_t chunkIndex = index / m_componentsPerChunk;
        const uint32_t indexInChunk = index % m_componentsPerChunk;
        auto chunk = get_chunk(chunkIndex);
        return chunk + indexInChunk;
    }

private:
    T* get_chunk(uint32_t chunkIndex) const {
        if (chunkIndex >= m_chunks.size()) {
            m_chunks.resize(chunkIndex + 1, nullptr);
        }
        auto& chunk = m_chunks[chunkIndex];
        if (!chunk) {
            chunk = (T*)malloc(sizeof(T) * m_componentsPerChunk);
        }
        return chunk;
    }

private:
    uint32_t m_componentsPerChunk;
    mutable std::vector<T*> m_chunks;
};

}// namespace detail

class Objects;

template<typename T, bool isConst>
class ComponentHandle;

using ComponentMask = duk::tools::BitBlock<detail::kMaxComponents>;

class Id {
public:
    Id();

    explicit Id(uint32_t index, uint32_t version);

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t version() const;

    bool operator==(const Id& rhs) const;

    bool operator!=(const Id& rhs) const;

private:
    uint32_t m_index;
    uint32_t m_version;
};

template<bool isConst>
class ObjectHandle {
public:
    using ObjectsType = duk::tools::maybe_const_t<Objects, isConst>;

    ObjectHandle();

    ObjectHandle(uint32_t index, uint32_t version, ObjectsType* objects);

    DUK_NO_DISCARD Id id() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD operator bool() const;

    DUK_NO_DISCARD ObjectsType* objects() const;

    void destroy() const;

    const ComponentMask& component_mask() const;

    template<typename T, typename... Args>
    ComponentHandle<T, isConst> add(Args&&... args);

    template<typename T>
    void remove();

    template<typename T>
    ComponentHandle<T, isConst> component();

    template<typename T>
    const ComponentHandle<T, isConst> component() const;

    template<typename... Ts>
    std::tuple<ComponentHandle<Ts, isConst>...> components();

    template<typename... Ts>
    std::tuple<const ComponentHandle<Ts, isConst>...> components() const;

private:
    Id m_id;
    ObjectsType* m_objects;
};

using Object = ObjectHandle<false>;

using ConstObject = ObjectHandle<true>;

template<typename T, bool isConst>
class ComponentHandle {
public:
    using ObjectsType = duk::tools::maybe_const_t<Objects, isConst>;
    using Type = duk::tools::maybe_const_t<T, isConst>;

    ComponentHandle(const Id& ownerId, ObjectsType* objects);

    ComponentHandle(const ObjectHandle<isConst>& owner);

    ComponentHandle();

    Type* operator->();

    Type* operator->() const;

    Type& operator*();

    Type& operator*() const;

    Type* get();

    Type* get() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD explicit operator bool() const;

    DUK_NO_DISCARD ObjectHandle<isConst> object() const;

private:
    Id m_ownerId;
    ObjectsType* m_objects;
};

template<typename T>
using Component = ComponentHandle<T, false>;

template<typename T>
using ConstComponent = ComponentHandle<T, true>;

class ComponentRegistry {
private:
    // Unfortunately we have to duplicate the solve method for each solver type.
    // At the moment I could not think of a more elegant solution, this is not as simple
    // as it might look at first glance :(
    class ComponentEntry {
    public:
        virtual ~ComponentEntry() = default;

        virtual void copy(const ObjectHandle<true>& src, ObjectHandle<false>& dst) = 0;

        virtual void solve(duk::resource::ReferenceSolver* solver, ObjectHandle<false>& object) = 0;

        virtual void solve(duk::resource::DependencySolver* solver, ObjectHandle<false>& object) = 0;

        virtual void from_json(const rapidjson::Value& json, ObjectHandle<false>& object) = 0;

        virtual void to_json(rapidjson::Document& document, rapidjson::Value& json, const ObjectHandle<true>& object) = 0;

        virtual const std::string& name() const = 0;
    };

    template<typename T>
    class ComponentEntryT : public ComponentEntry {
    public:
        void copy(const ObjectHandle<true>& src, ObjectHandle<false>& dst) override {
            auto dstComponent = dst.add<T>();
            const auto srcComponent = src.component<T>();
            *dstComponent = *srcComponent;
        }

        void solve(duk::resource::ReferenceSolver* solver, ObjectHandle<false>& object) override {
            auto component = ComponentHandle<T, false>(object);
            solver->solve(component);
        }

        void solve(duk::resource::DependencySolver* solver, ObjectHandle<false>& object) override {
            auto component = ComponentHandle<T, false>(object);
            solver->solve(component);
        }

        void from_json(const rapidjson::Value& json, ObjectHandle<false>& object) override {
            duk::serial::from_json(json, *object.add<T>());
        }

        void to_json(rapidjson::Document& document, rapidjson::Value& json, const ObjectHandle<true>& object) override {
            duk::serial::to_json(document, json, *object.component<T>());
        }

        const std::string& name() const override {
            return duk::tools::type_name_of<T>();
        }
    };

public:
    static ComponentRegistry* instance();

    ComponentRegistry()
        : m_componentIndexCounter(0) {
    }

    void copy_component(const ObjectHandle<true>& src, ObjectHandle<false>& dst, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->copy(src, dst);
    }

    template<typename Solver>
    void solve(Solver* solver, ObjectHandle<false>& object, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->solve(solver, object);
    }

    void from_json(const rapidjson::Value& json, ObjectHandle<false>& object, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->from_json(json, object);
    }

    void from_json(const rapidjson::Value& json, ObjectHandle<false>& object, const std::string& componentName) {
        const auto it = m_componentNameToIndex.find(componentName);
        if (it == m_componentNameToIndex.end()) {
            duk::log::warn("Unregistered Component type: \"{}\"", componentName);
            return;
        }
        const auto index = it->second;
        from_json(json, object, index);
    }

    void from_json(const rapidjson::Value& json, ObjectHandle<false>& object) {
        std::string type;
        duk::serial::from_json_member(json, "type", type);
        from_json(json, object, type);
    }

    void to_json(rapidjson::Document& document, rapidjson::Value& json, const ObjectHandle<true>& object, uint32_t componentIndex) {
        const auto& type = name_of(componentIndex);
        duk::serial::to_json_member(document, json, "type", type);
        m_componentEntries.at(componentIndex)->to_json(document, json, object);
    }

    template<typename T>
    void add() {
        const auto& name = duk::tools::type_name_of<T>();
        auto it = m_componentNameToIndex.find(name);
        if (it != m_componentNameToIndex.end()) {
            return;
        }
        const auto index = m_componentIndexCounter++;
        m_componentEntries.at(index) = std::make_unique<ComponentEntryT<T>>();
        m_componentNameToIndex.emplace(name, index);
    }

    const std::string& name_of(uint32_t index) const;

    uint32_t index_of(const std::string& componentTypeName) const;

    template<typename T>
    uint32_t index_of() const;

private:
    uint32_t m_componentIndexCounter;
    std::array<std::unique_ptr<ComponentEntry>, detail::kMaxComponents> m_componentEntries;
    std::unordered_map<std::string, uint32_t> m_componentNameToIndex;
};

template<typename T>
uint32_t ComponentRegistry::index_of() const {
    static const uint32_t index = index_of(duk::tools::type_name_of<T>());
    return index;
}

template<typename T>
void register_component() {
    ComponentRegistry::instance()->add<T>();
}

class Objects {
public:
    template<bool isConst>
    class ObjectView {
    public:
        using ObjectsType = duk::tools::maybe_const_t<Objects, isConst>;

        class Iterator {
        public:
            Iterator(uint32_t index, ObjectsType* objects, ComponentMask componentMask);
            // Dereference operator (*)
            DUK_NO_DISCARD ObjectHandle<isConst> operator*() const;

            // Pre-increment operator (++it)
            Iterator& operator++();

            // Post-increment operator (it++)
            DUK_NO_DISCARD Iterator operator++(int);

            // Sum operator (it + value)
            DUK_NO_DISCARD Iterator operator+(int value) const;

            // Equality operator (it1 == it2)
            DUK_NO_DISCARD bool operator==(const Iterator& other) const;

            // Inequality operator (it1 != it2)
            DUK_NO_DISCARD bool operator!=(const Iterator& other) const;

        private:
            void next();

            bool valid_object();

        private:
            uint32_t m_i;
            uint32_t m_freeListCursor;
            ObjectsType* m_objects;
            ComponentMask m_componentMask;
        };

    public:
        ObjectView(ObjectsType* objects, ComponentMask componentMask);

        DUK_NO_DISCARD Iterator begin();

        DUK_NO_DISCARD Iterator begin() const;

        DUK_NO_DISCARD Iterator end();

        DUK_NO_DISCARD Iterator end() const;

    private:
        ObjectsType* m_objects;
        ComponentMask m_componentMask;
    };

public:
    ~Objects();

    ObjectHandle<false> add_object();

    /// builds a new object which is an exact copy of the original object
    ObjectHandle<false> copy_object(const ObjectHandle<true>& src);

    ObjectHandle<false> copy_objects(const Objects& src);

    void destroy_object(const Id& id);

    DUK_NO_DISCARD ObjectHandle<false> object(const Id& id);

    DUK_NO_DISCARD ObjectHandle<true> object(const Id& id) const;

    DUK_NO_DISCARD bool valid_object(const Id& id) const;

    DUK_NO_DISCARD ObjectView<false> all();

    DUK_NO_DISCARD ObjectView<true> all() const;

    template<typename... Ts>
    DUK_NO_DISCARD ObjectView<false> all_with();

    template<typename... Ts>
    DUK_NO_DISCARD ObjectView<true> all_with() const;

    template<typename... Ts>
    DUK_NO_DISCARD ObjectHandle<false> first_with();

    template<typename... Ts>
    DUK_NO_DISCARD ObjectHandle<true> first_with() const;

    DUK_NO_DISCARD const ComponentMask& component_mask(const Id& id) const;

    template<typename T, typename... Args>
    void add_component(const Id& id, Args&&... args);

    template<typename T>
    void remove_component(const Id& id);

    template<typename T>
    DUK_NO_DISCARD T* component(const Id& id);

    template<typename T>
    DUK_NO_DISCARD const T* component(const Id& id) const;

    template<typename T>
    DUK_NO_DISCARD bool valid_component(const Id& id) const;

    void update();

private:
    template<typename T>
    detail::ComponentPoolT<T>* pool();

    template<typename T>
    const detail::ComponentPoolT<T>* pool() const;

    template<typename T>
    static ComponentMask component_mask();

    template<typename T1, typename T2, typename... Ts>
    static ComponentMask component_mask();

    void remove_component(uint32_t index, uint32_t componentIndex);

private:
    std::array<std::unique_ptr<detail::ComponentPool>, detail::kMaxComponents> m_componentPools;
    std::vector<ComponentMask> m_componentMasks;
    std::vector<uint32_t> m_versions;
    std::vector<uint32_t> m_freeList;
    std::vector<Id> m_destroyedIds;
};

using ObjectsResource = duk::resource::ResourceT<Objects>;

// Object Implementation //

template<bool isConst>
ObjectHandle<isConst>::ObjectHandle()
    : ObjectHandle(detail::kMaxObjects, 0, nullptr) {
}

template<bool isConst>
ObjectHandle<isConst>::ObjectHandle(uint32_t index, uint32_t version, ObjectsType* objects)
    : m_id(index, version)
    , m_objects(objects) {
}

template<bool isConst>
Id ObjectHandle<isConst>::id() const {
    return m_id;
}

template<bool isConst>
bool ObjectHandle<isConst>::valid() const {
    return m_objects != nullptr && m_objects->valid_object(m_id);
}

template<bool isConst>
ObjectHandle<isConst>::operator bool() const {
    return valid();
}

template<bool isConst>
typename ObjectHandle<isConst>::ObjectsType* ObjectHandle<isConst>::objects() const {
    return m_objects;
}

template<bool isConst>
void ObjectHandle<isConst>::destroy() const {
    m_objects->destroy_object(m_id);
}

template<bool isConst>
const ComponentMask& ObjectHandle<isConst>::component_mask() const {
    return m_objects->component_mask(m_id);
}

template<bool isConst>
template<typename T, typename... Args>
ComponentHandle<T, isConst> ObjectHandle<isConst>::add(Args&&... args) {
    m_objects->template add_component<T>(m_id, std::forward<Args>(args)...);
    return component<T>();
}

template<bool isConst>
template<typename T>
void ObjectHandle<isConst>::remove() {
    m_objects->template remove_component<T>(m_id);
}

template<bool isConst>
template<typename T>
ComponentHandle<T, isConst> ObjectHandle<isConst>::component() {
    return ComponentHandle<T, isConst>(m_id, m_objects);
}

template<bool isConst>
template<typename T>
const ComponentHandle<T, isConst> ObjectHandle<isConst>::component() const {
    return ComponentHandle<T, isConst>(m_id, m_objects);
}

template<bool isConst>
template<typename... Ts>
std::tuple<ComponentHandle<Ts, isConst>...> ObjectHandle<isConst>::components() {
    return std::make_tuple(component<Ts>()...);
}

template<bool isConst>
template<typename... Ts>
std::tuple<const ComponentHandle<Ts, isConst>...> ObjectHandle<isConst>::components() const {
    return std::make_tuple(component<Ts>()...);
}

// Component Implementation //

template<typename T, bool isConst>
ComponentHandle<T, isConst>::ComponentHandle(const Id& ownerId, ObjectsType* objects)
    : m_ownerId(ownerId)
    , m_objects(objects) {
}

template<typename T, bool isConst>
ComponentHandle<T, isConst>::ComponentHandle(const ObjectHandle<isConst>& owner)
    : m_ownerId(owner.id())
    , m_objects(owner.objects()) {
}

template<typename T, bool isConst>
ComponentHandle<T, isConst>::ComponentHandle()
    : m_ownerId()
    , m_objects(nullptr) {
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type* ComponentHandle<T, isConst>::operator->() {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type* ComponentHandle<T, isConst>::operator->() const {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type& ComponentHandle<T, isConst>::operator*() {
    DUK_ASSERT(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type& ComponentHandle<T, isConst>::operator*() const {
    DUK_ASSERT(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type* ComponentHandle<T, isConst>::get() const {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
typename ComponentHandle<T, isConst>::Type* ComponentHandle<T, isConst>::get() {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T, bool isConst>
bool ComponentHandle<T, isConst>::valid() const {
    return m_objects->template valid_component<T>(m_ownerId);
}

template<typename T, bool isConst>
ComponentHandle<T, isConst>::operator bool() const {
    return valid();
}

template<typename T, bool isConst>
ObjectHandle<isConst> ComponentHandle<T, isConst>::object() const {
    return m_objects->object(m_ownerId);
}

// Objects Implementation //

template<bool IsConst>
Objects::ObjectView<IsConst>::Iterator::Iterator(uint32_t index, ObjectsType* objects, ComponentMask componentMask)
    : m_i(index)
    , m_freeListCursor(0)
    , m_objects(objects)
    , m_componentMask(componentMask) {
    next();
}

template<bool isConst>
ObjectHandle<isConst> Objects::ObjectView<isConst>::Iterator::operator*() const {
    return m_objects->object(Id{m_i, m_objects->m_versions[m_i]});
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator& Objects::ObjectView<IsConst>::Iterator::operator++() {
    m_i++;
    next();
    return *this;
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::Iterator::operator++(int) {
    auto old = *this;
    m_i++;
    next();
    return old;
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::Iterator::operator+(int value) const {
    return Iterator(m_i + value, m_objects, m_componentMask);
}

template<bool IsConst>
bool Objects::ObjectView<IsConst>::Iterator::operator==(const Iterator& other) const {
    return m_i == other.m_i && m_objects == other.m_objects;
}

template<bool IsConst>
bool Objects::ObjectView<IsConst>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

template<bool IsConst>
void Objects::ObjectView<IsConst>::Iterator::next() {
    while (m_i < m_objects->m_versions.size() && !valid_object()) {
        m_i++;
        m_freeListCursor++;
    }
}

template<bool IsConst>
bool Objects::ObjectView<IsConst>::Iterator::valid_object() {
    if ((m_componentMask & m_objects->m_componentMasks[m_i]) != m_componentMask) {
        return false;
    }
    auto& freeList = m_objects->m_freeList;
    if (freeList.empty() || m_freeListCursor >= freeList.size()) {
        return true;
    }

    return freeList[m_freeListCursor] != m_i;
}

template<bool IsConst>
Objects::ObjectView<IsConst>::ObjectView(ObjectsType* objects, ComponentMask componentMask)
    : m_objects(objects)
    , m_componentMask(componentMask) {
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::begin() {
    return {0, m_objects, m_componentMask};
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::begin() const {
    return {0, m_objects, m_componentMask};
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::end() {
    return {static_cast<uint32_t>(m_objects->m_versions.size()), m_objects, m_componentMask};
}

template<bool IsConst>
typename Objects::ObjectView<IsConst>::Iterator Objects::ObjectView<IsConst>::end() const {
    return {static_cast<uint32_t>(m_objects->m_versions.size()), m_objects, m_componentMask};
}

template<typename... Ts>
Objects::ObjectView<false> Objects::all_with() {
    return ObjectView<false>(this, component_mask<Ts...>());
}

template<typename... Ts>
Objects::ObjectView<true> Objects::all_with() const {
    return ObjectView<true>(this, component_mask<Ts...>());
}

template<typename... Ts>
ObjectHandle<false> Objects::first_with() {
    for (auto object: all_with<Ts...>()) {
        return object;
    }
    return ObjectHandle<false>();
}

template<typename... Ts>
ObjectHandle<true> Objects::first_with() const {
    for (auto object: all_with<Ts...>()) {
        return object;
    }
    return ObjectHandle<true>();
}

template<typename T, typename... Args>
void Objects::add_component(const Id& id, Args&&... args) {
    DUK_ASSERT(valid_object(id));
    DUK_ASSERT(!valid_component<T>(id));
    auto componentPool = pool<T>();
    componentPool->construct(id.index(), std::forward<Args>(args)...);
    m_componentMasks[id.index()].set(ComponentRegistry::instance()->index_of<T>());
}

template<typename T>
void Objects::remove_component(const Id& id) {
    DUK_ASSERT(valid_component<T>(id));
    const auto index = ComponentRegistry::instance()->index_of<T>();
    remove_component(id.index(), index);
    m_componentMasks[id.index()].reset(index);
}

template<typename T>
T* Objects::component(const Id& id) {
    DUK_ASSERT(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
const T* Objects::component(const Id& id) const {
    DUK_ASSERT(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
bool Objects::valid_component(const Id& id) const {
    if (!valid_object(id)) {
        return false;
    }
    const auto index = ComponentRegistry::instance()->index_of<T>();
    return m_componentMasks[id.index()].test(index);
}

template<typename T>
detail::ComponentPoolT<T>* Objects::pool() {
    const auto index = ComponentRegistry::instance()->index_of<T>();
    auto& pool = m_componentPools[index];
    if (!pool) {
        pool = std::make_unique<detail::ComponentPoolT<T>>(detail::kComponentsPerChunk);
    }

    auto componentPool = dynamic_cast<detail::ComponentPoolT<T>*>(pool.get());
    if (!componentPool) {
        throw std::logic_error("invalid pool type allocated");
    }

    return componentPool;
}

template<typename T>
const detail::ComponentPoolT<T>* Objects::pool() const {
    const auto index = ComponentRegistry::instance()->index_of<T>();
    const auto& pool = m_componentPools[index];
    if (!pool) {
        return nullptr;
    }
    return dynamic_cast<const detail::ComponentPoolT<T>*>(pool.get());
}

template<typename T>
ComponentMask Objects::component_mask() {
    ComponentMask mask;
    mask.set(ComponentRegistry::instance()->index_of<T>());
    return mask;
}

template<typename T1, typename T2, typename... Ts>
ComponentMask Objects::component_mask() {
    return component_mask<T1>() | component_mask<T2, Ts...>();
}

}// namespace duk::objects

namespace duk::serial {

template<>
inline void from_json<duk::objects::ObjectHandle<false>>(const rapidjson::Value& json, duk::objects::ObjectHandle<false>& object) {
    DUK_ASSERT(json.IsObject());
    auto jsonComponentsArray = json["components"].GetArray();
    for (auto& jsonComponent: jsonComponentsArray) {
        std::string type;
        from_json_member(jsonComponent, "type", type);
        objects::ComponentRegistry::instance()->from_json(jsonComponent, object, type);
    }
}

template<>
inline void to_json<duk::objects::ObjectHandle<true>>(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::ObjectHandle<true>& object) {
    rapidjson::Value jsonComponents;
    auto jsonComponentsArray = jsonComponents.SetArray().GetArray();
    auto registry = objects::ComponentRegistry::instance();
    for (auto componentIndex: object.component_mask().bits<true>()) {
        rapidjson::Value jsonComponent;
        registry->to_json(document, jsonComponent, object, componentIndex);
        jsonComponentsArray.PushBack(jsonComponent, document.GetAllocator());
    }
    json.SetObject();
    json.AddMember("components", std::move(jsonComponentsArray), document.GetAllocator());
}

template<>
inline void from_json<duk::objects::Objects>(const rapidjson::Value& json, duk::objects::Objects& objects) {
    DUK_ASSERT(json.IsArray());
    auto jsonArray = json.GetArray();
    for (auto& jsonElement: jsonArray) {
        auto object = objects.add_object();
        from_json(jsonElement, object);
    }
}

template<>
inline void to_json<duk::objects::Objects>(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::Objects& objects) {
    auto jsonArray = json.SetArray().GetArray();
    for (auto object: objects.all()) {
        rapidjson::Value jsonElement;
        to_json(document, jsonElement, object);
        jsonArray.PushBack(std::move(jsonElement), document.GetAllocator());
    }
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver, typename T>
void solve_resources(Solver* solver, duk::objects::ComponentHandle<T, false>& component) {
    solver->solve(*component);
}

template<typename Solver>
void solve_resources(Solver* solver, duk::objects::ObjectHandle<false>& object) {
    auto componentRegistry = duk::objects::ComponentRegistry::instance();

    const auto& componentMask = object.component_mask();

    for (auto componentId: componentMask.bits<true>()) {
        componentRegistry->solve(solver, object, componentId);
    }
}

template<typename Solver>
void solve_resources(Solver* solver, duk::objects::Objects& objects) {
    for (auto object: objects.all()) {
        solver->solve(object);
    }
}

}// namespace duk::resource

#endif// DUK_OBJECTS_OBJECTS_H
