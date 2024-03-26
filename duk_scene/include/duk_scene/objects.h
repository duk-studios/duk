/// 06/07/2023
/// scene.h

#ifndef DUK_SCENE_OBJECTS_H
#define DUK_SCENE_OBJECTS_H

#include <array>
#include <duk_log/log.h>
#include <duk_macros/assert.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>
#include <duk_scene/component_pool.h>
#include <duk_scene/limits.h>
#include <duk_serial/json_serializer.h>
#include <duk_tools/bit_block.h>
#include <duk_tools/fixed_vector.h>
#include <duk_tools/singleton.h>
#include <duk_tools/types.h>

namespace duk::scene {

namespace detail {

static constexpr uint32_t kComponentsPerChunk = 32;

}

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

public:
    Object();

    Object(uint32_t index, uint32_t version, Objects* scene);

    DUK_NO_DISCARD Id id() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD operator bool() const;

    DUK_NO_DISCARD Objects* scene() const;

    void destroy() const;

    const ComponentMask& component_mask();

    template<typename T, typename... Args>
    Component<T> add(Args&&... args);

    template<typename T>
    void remove();

    template<typename T>
    Component<T> component();

    template<typename T>
    Component<T> component() const;

    template<typename... Ts>
    std::tuple<Component<Ts>...> components();

private:
    Id m_id;
    Objects* m_objects;
};

template<typename T>
class Component {
public:
    Component(const Object::Id& ownerId, Objects* objects);

    Component(const Object& owner);

    Component();

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

class ComponentRegistry {
private:
    // Unfortunately we have to duplicate the solve method for each solver type.
    // At the moment I could not think of a more elegant solution, this is not as simple
    // as it might look at first glance :(
    class ComponentEntry {
    public:
        virtual ~ComponentEntry() = default;

        virtual void solve(duk::resource::ReferenceSolver* solver, Object& object) = 0;

        virtual void solve(duk::resource::DependencySolver* solver, Object& object) = 0;

        virtual void visit(duk::serial::JsonReader* serializer, Object& object) = 0;

        virtual void visit(duk::serial::JsonWriter* serializer, Object& object) = 0;

        virtual const std::string& name() const = 0;
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

        void visit(duk::serial::JsonReader* reader, Object& object) override {
            visit_object(reader, *object.add<T>());
        }

        void visit(duk::serial::JsonWriter* writer, Object& object) override {
            visit_object(writer, *object.component<T>());
        }

        const std::string& name() const override {
            return duk::tools::type_name_of<T>();
        }
    };

public:
    static ComponentRegistry* instance();

    template<typename Solver>
    void solve(Solver* solver, Object& object, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->solve(solver, object);
    }

    template<typename JsonVisitor>
    void visit(JsonVisitor* visitor, Object& object, uint32_t componentId) {
        auto& entry = m_componentEntries.at(componentId);
        assert(entry);
        m_componentEntries.at(componentId)->visit(visitor, object);
    }

    template<typename JsonVisitor>
    void visit(JsonVisitor* visitor, Object& object, const std::string& componentName) {
        const auto it = m_componentNameToIndex.find(componentName);
        if (it == m_componentNameToIndex.end()) {
            duk::log::warn("Unregistered Component type: \"{}\"", componentName);
            return;
        }
        const auto index = it->second;
        visit(visitor, object, index);
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
    std::array<std::unique_ptr<ComponentEntry>, MAX_COMPONENTS> m_componentEntries;
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

    template<typename... Ts>
    DUK_NO_DISCARD ObjectView all_with();

    template<typename... Ts>
    DUK_NO_DISCARD Object first_with();

    DUK_NO_DISCARD const ComponentMask& component_mask(const Object::Id& id) const;

    template<typename T, typename... Args>
    void add_component(const Object::Id& id, Args&&... args);

    template<typename T>
    void remove_component(const Object::Id& id);

    template<typename T>
    DUK_NO_DISCARD T* component(const Object::Id& id);

    template<typename T>
    DUK_NO_DISCARD T* component(const Object::Id& id) const;

    template<typename T>
    DUK_NO_DISCARD bool valid_component(const Object::Id& id) const;

private:
    friend class Systems;

    template<typename T>
    ComponentPoolT<T>* pool();

    template<typename T>
    static ComponentMask component_mask();

    template<typename T1, typename T2, typename... Ts>
    static ComponentMask component_mask();

    void remove_component(uint32_t index, uint32_t componentIndex);

    void update_destroy();

private:
    std::array<std::unique_ptr<ComponentPool>, MAX_COMPONENTS> m_componentPools;
    std::vector<ComponentMask> m_componentMasks;
    std::vector<uint32_t> m_versions;
    std::vector<uint32_t> m_freeList;
    std::vector<Object::Id> m_destroyedIds;
};

// Component Implementation //

template<typename T>
Component<T>::Component(const Object::Id& ownerId, Objects* objects)
    : m_ownerId(ownerId)
    , m_objects(objects) {
}

template<typename T>
Component<T>::Component(const Object& owner)
    : m_ownerId(owner.id())
    , m_objects(owner.scene()) {
}

template<typename T>
Component<T>::Component()
    : m_ownerId()
    , m_objects(nullptr) {
}

template<typename T>
T* Component<T>::operator->() {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::operator->() const {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() {
    DUK_ASSERT(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() const {
    DUK_ASSERT(valid());
    return *m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() {
    DUK_ASSERT(valid());
    return m_objects->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() const {
    DUK_ASSERT(valid());
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
void Objects::add_component(const Object::Id& id, Args&&... args) {
    DUK_ASSERT(valid_object(id));
    DUK_ASSERT(!valid_component<T>(id));
    auto componentPool = pool<T>();
    componentPool->construct(id.index(), std::forward<Args>(args)...);
    m_componentMasks[id.index()].set(ComponentRegistry::instance()->index_of<T>());
}

template<typename T>
void Objects::remove_component(const Object::Id& id) {
    DUK_ASSERT(valid_component<T>(id));
    const auto index = ComponentRegistry::instance()->index_of<T>();
    remove_component(id.index(), index);
    m_componentMasks[id.index()].reset(index);
}

template<typename T>
T* Objects::component(const Object::Id& id) {
    DUK_ASSERT(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
T* Objects::component(const Object::Id& id) const {
    DUK_ASSERT(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
bool Objects::valid_component(const Object::Id& id) const {
    if (!valid_object(id)) {
        return false;
    }
    const auto index = ComponentRegistry::instance()->index_of<T>();
    return m_componentMasks[id.index()].test(index);
}

template<typename T>
ComponentPoolT<T>* Objects::pool() {
    const auto index = ComponentRegistry::instance()->index_of<T>();
    auto& pool = m_componentPools[index];
    if (!pool) {
        pool = std::make_unique<ComponentPoolT<T>>(detail::kComponentsPerChunk);
    }

    auto componentPool = dynamic_cast<ComponentPoolT<T>*>(pool.get());
    if (!componentPool) {
        throw std::logic_error("invalid pool type allocated");
    }

    return componentPool;
}

template<typename... Ts>
Objects::ObjectView Objects::all_with() {
    return Objects::ObjectView(this, component_mask<Ts...>());
}

template<typename... Ts>
Object Objects::first_with() {
    for (auto object: all_with<Ts...>()) {
        return object;
    }
    return Object();
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

// From object.h //

template<typename T, typename... Args>
Component<T> Object::add(Args&&... args) {
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

}// namespace duk::scene

namespace duk::serial {

// Object serialization
// As usual with arrays, we have to split our serialization into read and write.
// Our components also have to be wrapped into a SerializedComponent, otherwise
// we would not be able to serialize different types into a single array.
// Maybe one day we can have a better solution for arrays like this,
// but for now this is more than good enough
struct SerializedComponent {
    duk::scene::Object& object;
    std::string type;
};

class SerializedComponents {
public:
    using iterator = std::vector<SerializedComponent>::iterator;

    SerializedComponents(duk::scene::Object& object)
        : m_object(object) {
        auto registry = duk::scene::ComponentRegistry::instance();
        const auto& componentMask = object.component_mask();
        for (auto componentIndex: componentMask.bits<true>()) {
            m_components.emplace_back(m_object, registry->name_of(componentIndex));
        }
    }

    SerializedComponent& add() {
        return m_components.emplace_back(m_object);
    }

    iterator begin() {
        return m_components.begin();
    }

    iterator end() {
        return m_components.end();
    }

private:
    duk::scene::Object& m_object;
    std::vector<SerializedComponent> m_components;
};

template<typename JsonVisitor>
void visit_object(JsonVisitor* serializer, SerializedComponent& component) {
    serializer->visit_member(component.type, MemberDescription("type"));
    duk::scene::ComponentRegistry::instance()->visit(serializer, component.object, component.type);
}

template<>
inline void read_array<SerializedComponents>(JsonReader* reader, SerializedComponents& components, size_t size) {
    for (size_t i = 0; i < size; i++) {
        reader->visit_member_object(components.add(), i);
    }
}

template<>
inline void write_array(JsonWriter* writer, SerializedComponents& components) {
    for (auto& element: components) {
        writer->visit_member_object(element, MemberDescription(nullptr));
    }
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::scene::Object& object) {
    SerializedComponents serializedComponents(object);
    visitor->visit_member_array(serializedComponents, MemberDescription("components"));
}

// Objects
template<>
inline void read_array<duk::scene::Objects>(JsonReader* reader, duk::scene::Objects& objects, size_t size) {
    for (size_t i = 0; i < size; i++) {
        auto object = objects.add_object();
        reader->visit_member_object(object, MemberDescription(i));
    }
}

template<>
inline void write_array(JsonWriter* writer, duk::scene::Objects& objects) {
    for (auto object: objects.all()) {
        writer->visit_member_object(object, MemberDescription(nullptr));
    }
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver, typename T>
void solve_resources(Solver* solver, duk::scene::Component<T>& component) {
    solver->solve(*component);
}

template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Object& object) {
    auto componentRegistry = duk::scene::ComponentRegistry::instance();

    const auto& componentMask = object.component_mask();

    for (auto componentId: componentMask.bits<true>()) {
        componentRegistry->solve(solver, object, componentId);
    }
}

template<typename Solver>
void solve_resources(Solver* solver, duk::scene::Objects& objects) {
    for (auto object: objects.all()) {
        solver->solve(object);
    }
}

}// namespace duk::resource

#endif// DUK_SCENE_OBJECTS_H
