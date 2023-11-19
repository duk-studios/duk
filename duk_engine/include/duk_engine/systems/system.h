//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_SYSTEM_H
#define DUK_ENGINE_SYSTEM_H

#include <duk_macros/macros.h>

#include <string>
#include <memory>
#include <vector>

namespace duk::engine {

class Engine;

class System {
public:

    explicit System(Engine& engine, std::string name);

    virtual ~System();

    virtual void update() = 0;

    const std::string& name();

protected:

    DUK_NO_DISCARD Engine* engine();

    DUK_NO_DISCARD Engine* engine() const;

private:
    Engine& m_engine;
    std::string m_name;
};

class Systems {
public:
    using Container = std::vector<std::unique_ptr<System>>;
    using Iterator = Container::iterator;
    using ConstIterator = Container::const_iterator;

    template<typename T, typename ...Args>
    Iterator add_system(Args&&... args);

    template<typename T, typename ...Args>
    Iterator add_system(Iterator position, Args&&... args);

    DUK_NO_DISCARD Iterator begin();

    DUK_NO_DISCARD ConstIterator begin() const;

    DUK_NO_DISCARD Iterator end();

    DUK_NO_DISCARD ConstIterator end() const;

private:
    Container m_container;
};

template<typename T, typename... Args>
Systems::Iterator Systems::add_system(Args &&... args) {
    return add_system<T>(m_container.end(), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
Systems::Iterator Systems::add_system(Systems::Iterator position, Args &&... args) {
    auto newSystem = std::make_unique<T>(std::forward<Args>(args)...);
    const auto alreadyPresent = std::any_of(begin(), end(), [&newSystem](const auto& system) {
        return newSystem->name() == system->name();
    });
    if (alreadyPresent) {
        throw std::logic_error(newSystem->name() + "is already present");
    }
    return m_container.insert(position, std::move(newSystem));
}

}

#endif //DUK_ENGINE_SYSTEM_H
