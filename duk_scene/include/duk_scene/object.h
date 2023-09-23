/// 06/07/2023
/// object.h

#ifndef DUK_SCENE_OBJECT_H
#define DUK_SCENE_OBJECT_H

#include <duk_macros/macros.h>
#include <duk_scene/component_pool.h>

#include <cstdint>

namespace duk::scene {

class Scene;

template<typename T>
class Component;

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

    Object(uint32_t index, uint32_t version, Scene* scene);

    DUK_NO_DISCARD Id id() const;

    DUK_NO_DISCARD bool valid() const;

    DUK_NO_DISCARD operator bool() const;

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

private:
    Id m_id;
    Scene* m_scene;
};

}

#endif // DUK_SCENE_OBJECT_H
