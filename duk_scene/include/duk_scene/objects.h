/// 06/07/2023
/// scene.h

#ifndef DUK_SCENE_OBJECTS_H
#define DUK_SCENE_OBJECTS_H

#include <duk_scene/component_pool.h>
#include <duk_scene/limits.h>
#include <duk_tools/fixed_vector.h>
#include <duk_tools/bit_block.h>
#include <duk_tools/singleton.h>
#include <duk_resource/solver/reference_solver.h>
#include <duk_resource/solver/dependency_solver.h>

#include <array>
#include "duk_log/log.h"
#include "duk_json/string.h"

namespace duk::scene {

class Objects;

template<typename T>
class Component;

using ComponentMask = duk::tools::BitBlock<MAX_COMPONENTS>;

class Object {
public:

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

    class ComponentIterator {
    public:

        ComponentIterator(const ComponentMask& mask, uint32_t index);

        bool operator==(const ComponentIterator& rhs);

        bool operator!=(const ComponentIterator& rhs);

        ComponentIterator operator++();

        uint32_t operator*();

    private:

        void next();

    private:
        const ComponentMask& m_mask;
        uint32_t m_i;
    };

    class ComponentView {
    public:

        ComponentView(const ComponentMask& mask);

        ComponentIterator begin();

        ComponentIterator end();

    private:
        const ComponentMask& m_mask;
    };

public:

    Object();

    Object(uint32_t index, uint32_t version, Objects* scene);

    DUK_NO_DISCARD Id id() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD operator bool() const;

    DUK_NO_DISCARD Objects* scene() const;

    void destroy() const;

    template<typename T, typename ...Args>
    Component<T> add(Args&& ...args);

    template<typename T>
    void remove();

    template<typename T>
    Component<T> component();

    template<typename T>
    Component<T> component() const;

    template<typename ...Ts>
    std::tuple<Component<Ts>...> components();

    ComponentView components();

private:
    Id m_id;
    Objects* m_objects;
};


template<typename T>
class Component {
public:
    Component(const Object::Id& ownerId, Objects* objects);

    Component(const Object& owner);

    T* operator->();

    T* operator->() const;

    T& operator*();

    T& operator*() const;

    T* get();

    T* get() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD explicit operator bool() const;

    DUK_NO_DISCARD Object object() const;

private:
    Object::Id m_ownerId;
    Objects* m_objects;
};

class ComponentRegistry : public duk::tools::Singleton<ComponentRegistry> {
private:

    // Unfortunately we have to duplicate the solve method for each solver type.
    // At the moment I could not think of a more elegant solution, this is not as simple
    // as it might look at first glance :(
    class ComponentEntry {
    public:
        virtual ~ComponentEntry() = default;

        virtual void solve(duk::resource::ReferenceSolver* solver, Object& object) = 0;

        virtual void solve(duk::resource::DependencySolver* solver, Object& object) = 0;

        virtual void build(Object& object, const rapidjson::Value& jsonObject) = 0;

    };

    template<typename T>
    class ComponentEntryT : public ComponentEntry {
    public:
        void solve(duk::resource::ReferenceSolver* solver, Object& object) override {
            auto component = Component<T>(object);
            solver->solve(component);
        }

        void solve(duk::resource::DependencySolver* solver, Object& object) override {
            auto component = Component<T>(object);
            solver->solve(component);
        }

        void build(Object& object, const rapidjson::Value& jsonObject) override {
            object.add<T>(json::from_json<T>(jsonObject));
        }
    };

public:

    template<typename T>
    DUK_NO_DISCARD static uint32_t component_index();

    template<typename Solver>
    void solve(Solver* solver, Object& object, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->solve(solver, object);
    }

    void build_from_json(Object& object, const rapidjson::Value& jsonObject) {
        const auto typeName = json::from_json_member<const char*>(jsonObject, "type");
        if (!typeName) {
            duk::log::warn("Missing \"type\" field from Component json object: {}", duk::json::to_string(jsonObject));
            return;
        }
        const auto it = m_componentNameToIndex.find(typeName);
        if (it == m_componentNameToIndex.end()) {
            duk::log::warn("Unregistered Component type: \"{}\"", typeName);
            return;
        }
        const auto index = it->second;
        auto& entry = m_componentEntries.at(index);
        assert(entry);
        entry->build(object, jsonObject);
    }

    template<typename T>
    void add(const std::string& typeName) {
        const auto index = component_index<T>();
        auto& entry = m_componentEntries.at(index);
        if (entry) {
            return;
        }
        entry = std::make_unique<ComponentEntryT<T>>();
        m_componentNameToIndex.emplace(typeName, index);
    }

private:
    static uint32_t s_componentIndexCounter;
    std::array<std::unique_ptr<ComponentEntry>, MAX_COMPONENTS> m_componentEntries;
    std::unordered_map<std::string, uint32_t> m_componentNameToIndex;
};

template<typename T>
uint32_t ComponentRegistry::component_index() {
    static const auto index = s_componentIndexCounter++;
    return index;
}

class Objects {
public:

    class ObjectView {
    public:

        class Iterator {
        public:
            Iterator(uint32_t index, Objects* scene, ComponentMask componentMask);
            // Dereference operator (*)
            DUK_NO_DISCARD Object operator*() const;

            // Pre-increment operator (++it)
            Iterator& operator++();

            // Post-increment operator (it++)
            DUK_NO_DISCARD Iterator operator++(int);

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
            Objects* m_objects;
            ComponentMask m_componentMask;
        };

    public:

        ObjectView(Objects* scene, ComponentMask componentMask);

        DUK_NO_DISCARD Iterator begin();

        DUK_NO_DISCARD Iterator begin() const;

        DUK_NO_DISCARD Iterator end();

        DUK_NO_DISCARD Iterator end() const;

    private:
        Objects* m_objects;
        ComponentMask m_componentMask;
    };

public:

    ~Objects();

    Object add_object();

    void destroy_object(const Object::Id& id);

    DUK_NO_DISCARD Object object(const Object::Id& id);

    DUK_NO_DISCARD bool valid_object(const Object::Id& id) const;

    DUK_NO_DISCARD ObjectView all();

    template<typename ...Ts>
    DUK_NO_DISCARD ObjectView all_with();

    DUK_NO_DISCARD Object::ComponentView components(const Object::Id& id);

    template<typename T, typename ...Args>
    void add_component(const Object::Id& id, Args&& ...args);

    template<typename T>
    void remove_component(const Object::Id& id);

    template<typename T>
    DUK_NO_DISCARD T* component(const Object::Id& id);

    template<typename T>
    DUK_NO_DISCARD T* component(const Object::Id& id) const;

    template<typename T>
    DUK_NO_DISCARD bool valid_component(const Object::Id& id) const;

private:

    template<typename T>
    ComponentPoolT<T>* pool();

    template<typename T>
    static ComponentMask component_mask();

    template<typename T1, typename T2, typename ...Ts>
    static ComponentMask component_mask();

    void remove_component(uint32_t index, uint32_t componentIndex);

private:
    std::array<std::unique_ptr<ComponentPool>, MAX_COMPONENTS> m_componentPools;
    duk::tools::FixedVector<ComponentMask, MAX_OBJECTS> m_componentMasks;
    duk::tools::FixedVector<uint32_t, MAX_OBJECTS> m_versions;
    duk::tools::FixedVector<uint32_t, MAX_OBJECTS> m_freeList;

};


// Component Implementation //

template<typename T>
Component<T>::Component(const Object::Id& ownerId, Objects* objects) :
    m_ownerId(ownerId),
    m_objects(objects) {

}

template<typename T>
Component<T>::Component(const Object& owner) :
    m_ownerId(owner.id()),
    m_objects(owner.scene()){

}

template<typename T>
T* Component<T>::operator->() {
    assert(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::operator->() const {
    assert(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() {
    assert(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() const {
    assert(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() {
    assert(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() const {
    assert(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
bool Component<T>::valid() const {
    return m_objects->template valid_component<T>(m_ownerId);
}

template<typename T>
Object Component<T>::object() const {
    return m_objects->object(m_ownerId);
}

template<typename T>
Component<T>::operator bool() const {
    return valid();
}

// Objects Implementation //

template<typename T, typename... Args>
void Objects::add_component(const Object::Id& id, Args&& ... args) {
    assert(valid_object(id));
    assert(!valid_component<T>(id));
    auto componentPool = pool<T>();
    componentPool->construct(id.index(), std::forward<Args>(args)...);
    m_componentMasks[id.index()].set(ComponentRegistry::component_index<T>());
}

template<typename T>
void Objects::remove_component(const Object::Id& id) {
    assert(valid_component<T>(id));
    remove_component(id.index(), ComponentRegistry::component_index<T>());
    m_componentMasks[id.index()].reset(ComponentRegistry::component_index<T>());
}

template<typename T>
T* Objects::component(const Object::Id& id) {
    assert(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
T* Objects::component(const Object::Id& id) const {
    assert(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
bool Objects::valid_component(const Object::Id& id) const {
    if (!valid_object(id)) {
        return false;
    }
    const auto componentIndex = ComponentRegistry::component_index<T>();
    return m_componentMasks[id.index()].test(componentIndex);
}

template<typename T>
ComponentPoolT<T>* Objects::pool() {
    const auto componentIndex = ComponentRegistry::component_index<T>();
    auto& pool = m_componentPools[componentIndex];
    if (!pool) {
        pool = std::make_unique<ComponentPoolT<T>>(MAX_OBJECTS);
    }

    auto componentPool = dynamic_cast<ComponentPoolT<T>*>(pool.get());
    if (!componentPool) {
        throw std::logic_error("invalid pool type allocated");
    }

    return componentPool;
}

template<typename ...Ts>
Objects::ObjectView Objects::all_with() {
    return Objects::ObjectView(this, component_mask<Ts...>());
}

template<typename T>
ComponentMask Objects::component_mask() {
    ComponentMask mask;
    mask.set(ComponentRegistry::component_index<T>());
    return mask;
}

template<typename T1, typename T2, typename ...Ts>
ComponentMask Objects::component_mask() {
    return component_mask<T1>() | component_mask<T2, Ts...>();
}

// From object.h //

template<typename T, typename... Args>
Component<T> Object::add(Args&& ... args) {
    m_objects->template add_component<T>(m_id, std::forward<Args>(args)...);
    return Component<T>(m_id, m_objects);
}

template<typename T>
void Object::remove() {
    m_objects->template remove_component<T>(m_id);
}

template<typename T>
Component<T> Object::component() {
    return Component<T>(m_id, m_objects);
}

template<typename T>
Component<T> Object::component() const {
    return Component<T>(m_id, m_objects);
}

template<typename... Ts>
std::tuple<Component<Ts>...> Object::components() {
    return std::make_tuple(component<Ts>()...);
}

}

namespace duk::resource {

template<typename Solver, typename T>
void solve_resources(Solver* solver, duk::scene::Component<T>& component) {
    solver->solve(*component);
}

template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Object& object) {

    auto componentRegistry = duk::scene::ComponentRegistry::instance(true);

    for (auto componentId : object.components()) {
        componentRegistry->solve(solver, object, componentId);
    }

}


template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Objects& scene) {
    for (auto object : scene.all()) {
        solver->solve(object);
    }
}

}

#endif // DUK_SCENE_OBJECTS_H
