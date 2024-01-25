/// 06/07/2023
/// scene.h

#ifndef DUK_SCENE_SCENE_H
#define DUK_SCENE_SCENE_H

#include <duk_scene/object.h>
#include <duk_scene/component_pool.h>
#include <duk_scene/limits.h>
#include <duk_tools/fixed_vector.h>
#include <duk_tools/bit_block.h>

#include <array>

namespace duk::scene {

class Scene;

template<typename T>
class Component {
public:
    Component(const Object::Id& ownerId, Scene* scene);

    T* operator->();

    T* operator->() const;

    T& operator*();

    T& operator*() const;

    T* get();

    T* get() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD explicit operator bool() const;

    DUK_NO_DISCARD Object owner() const;

private:
    Object::Id m_ownerId;
    Scene* m_scene;
};


using ComponentMask = duk::tools::BitBlock<MAX_COMPONENTS>;

class Scene {
public:

    class ObjectView {
    public:

        class Iterator {
        public:
            Iterator(uint32_t index, Scene* scene, ComponentMask componentMask);
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
            Scene* m_scene;
            ComponentMask m_componentMask;
        };

    public:

        ObjectView(Scene* scene, ComponentMask componentMask);

        DUK_NO_DISCARD Iterator begin();

        DUK_NO_DISCARD Iterator begin() const;

        DUK_NO_DISCARD Iterator end();

        DUK_NO_DISCARD Iterator end() const;

    private:
        Scene* m_scene;
        ComponentMask m_componentMask;
    };

public:

    ~Scene();

    Object add_object();

    void destroy_object(const Object::Id& id);

    DUK_NO_DISCARD Object object(const Object::Id& id);

    DUK_NO_DISCARD bool valid_object(const Object::Id& id) const;

    DUK_NO_DISCARD ObjectView objects();

    template<typename ...Ts>
    DUK_NO_DISCARD ObjectView objects_with_components();

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
    DUK_NO_DISCARD static uint32_t component_index();

    template<typename T>
    static ComponentMask component_mask();

    template<typename T1, typename T2, typename ...Ts>
    static ComponentMask component_mask();

    void remove_component(uint32_t index, uint32_t componentIndex);

private:
    static uint32_t s_componentIndexCounter;
    std::array<std::unique_ptr<ComponentPool>, MAX_COMPONENTS> m_componentPools;
    duk::tools::FixedVector<ComponentMask, MAX_OBJECTS> m_objectComponentMasks;
    duk::tools::FixedVector<uint32_t, MAX_OBJECTS> m_objectVersions;
    duk::tools::FixedVector<uint32_t, MAX_OBJECTS> m_freeList;

};

// Component Implementation //

template<typename T>
Component<T>::Component(const Object::Id& ownerId, Scene* scene) :
    m_ownerId(ownerId),
    m_scene(scene) {

}

template<typename T>
T* Component<T>::operator->() {
    assert(valid());
    return m_scene->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::operator->() const {
    assert(valid());
    return m_scene->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() {
    assert(valid());
    return *m_scene->template component<T>(m_ownerId);
}

template<typename T>
T& Component<T>::operator*() const {
    assert(valid());
    return *m_scene->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() {
    assert(valid());
    return m_scene->template component<T>(m_ownerId);
}

template<typename T>
T* Component<T>::get() const {
    assert(valid());
    return m_scene->template component<T>(m_ownerId);
}

template<typename T>
bool Component<T>::valid() const {
    return m_scene->template valid_component<T>(m_ownerId);
}

template<typename T>
Object Component<T>::owner() const {
    return m_scene->object(m_ownerId);
}

template<typename T>
Component<T>::operator bool() const {
    return valid();
}

// Scene Implementation //

template<typename T, typename... Args>
void Scene::add_component(const Object::Id& id, Args&& ... args) {
    assert(valid_object(id));
    auto componentPool = pool<T>();
    componentPool->construct(id.index(), std::forward<Args>(args)...);
    m_objectComponentMasks[id.index()].set(component_index<T>());
}

template<typename T>
void Scene::remove_component(const Object::Id& id) {
    assert(contains<T>(id));
    remove_component(id.index(), component_index<T>());
    m_objectComponentMasks[id.index()].reset(component_index<T>());
}

template<typename T>
T* Scene::component(const Object::Id& id) {
    assert(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
T* Scene::component(const Object::Id& id) const {
    assert(valid_component<T>(id));
    auto componentPool = pool<T>();
    return componentPool->get(id.index());
}

template<typename T>
bool Scene::valid_component(const Object::Id& id) const {
    if (!valid_object(id)) {
        return false;
    }
    const auto componentIndex = component_index<T>();
    return m_objectComponentMasks[id.index()].test(componentIndex);
}

template<typename T>
ComponentPoolT<T>* Scene::pool() {
    const auto componentIndex = component_index<T>();
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

template<typename T>
uint32_t Scene::component_index() {
    static auto componentIndex = s_componentIndexCounter++;
    return componentIndex;
}

template<typename ...Ts>
Scene::ObjectView Scene::objects_with_components() {
    return Scene::ObjectView(this, component_mask<Ts...>());
}

template<typename T>
ComponentMask Scene::component_mask() {
    ComponentMask mask;
    mask.set(component_index<T>());
    return mask;
}

template<typename T1, typename T2, typename ...Ts>
ComponentMask Scene::component_mask() {
    return component_mask<T1>() | component_mask<T2, Ts...>();
}

// From object.h //

template<typename T, typename... Args>
Component<T> Object::add(Args&& ... args) {
    m_scene->template add_component<T>(m_id, std::forward<Args>(args)...);
    return Component<T>(m_id, m_scene);
}

template<typename T>
void Object::remove() {
    m_scene->template remove_component<T>(m_id);
}

template<typename T>
Component<T> Object::component() {
    return Component<T>(m_id, m_scene);
}

template<typename T>
Component<T> Object::component() const {
    return Component<T>(m_id, m_scene);
}

template<typename... Ts>
std::tuple<Component<Ts>...> Object::components() {
    return std::make_tuple(component<Ts>()...);
}

}

#endif // DUK_SCENE_SCENE_H
