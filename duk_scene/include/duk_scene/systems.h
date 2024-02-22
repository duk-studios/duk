/// 21/02/2024
/// system.h

#ifndef DUK_SCENE_SYSTEMS_H
#define DUK_SCENE_SYSTEMS_H

#include <duk_scene/objects.h>
#include <duk_tools/types.h>

namespace duk::scene {

class System {
public:
    virtual ~System();

    virtual void enter() = 0;

    virtual void update() = 0;

    virtual void exit() = 0;
};

class Systems;

class SystemRegistry : public duk::tools::Singleton<SystemRegistry> {
private:
    class SystemEntry {
    public:
        virtual ~SystemEntry() = default;

        virtual void build(Systems& systems) = 0;
    };

    template<typename T, typename... Args>
    class SystemEntryT : public SystemEntry {
    public:
        SystemEntryT(Args&&... args);

        void build(Systems& systems);

    private:
        std::tuple<Args...> m_buildArgs;
    };

public:
    template<typename T>
    static size_t index();

    template<typename T, typename... Args>
    void register_system(Args&&... args);

    void build(const std::string& entryName, Systems& systems);

private:
    static size_t s_entryCounter;
    std::vector<std::unique_ptr<SystemEntry>> m_entries;
    std::unordered_map<std::string, size_t> m_entryNameToIndex;
};

class Systems {
public:
    class SystemIterator {
    public:
        SystemIterator(Systems& systems, size_t i);

        SystemIterator& operator++();

        System* operator*();

        bool operator==(const SystemIterator& rhs);

        bool operator!=(const SystemIterator& rhs);

    private:
        Systems& m_systems;
        size_t m_i;
    };

public:
    template<typename T, typename... Args>
    void add(Args&&... args);

    template<typename T>
    T* get();

    void enter();

    void update();

    void exit();

    SystemIterator begin();

    SystemIterator end();

private:
    std::vector<std::unique_ptr<System>> m_container;
    std::unordered_map<size_t, size_t> m_entryIndexToContainerIndex;
};

namespace detail {
template<typename T, typename... Args>
static void build_system(Systems& systems, Args&&... args) {
    systems.add<T>(std::forward<Args>(args)...);
}
}// namespace detail

template<typename T, typename... Args>
SystemRegistry::SystemEntryT<T, Args...>::SystemEntryT(Args&&... args) : m_buildArgs(std::forward<Args>(args)...) {
}

template<typename T, typename... Args>
void SystemRegistry::SystemEntryT<T, Args...>::build(Systems& systems) {
    std::apply([&systems](Args&&... args) {
        systems.add<T>(std::forward<Args>(args)...);
    },
               m_buildArgs);
}

template<typename T>
size_t SystemRegistry::index() {
    static const auto index = s_entryCounter++;
    return index;
}

template<typename T, typename... Args>
void SystemRegistry::register_system(Args&&... args) {
    const auto index = SystemRegistry::index<T>();
    if (m_entries.size() <= index) {
        m_entries.resize(index + 1);
    }

    auto& entry = m_entries[index];
    if (!entry) {
        entry = std::make_unique<SystemEntryT<T, Args...>>(std::forward<Args>(args)...);
        m_entryNameToIndex.emplace(duk::tools::type_name_of<T>(), index);
    }
}

template<typename T, typename... Args>
void Systems::add(Args&&... args) {
    static const auto entryIndex = SystemRegistry::index<T>();
    const auto containerIndex = m_container.size();
    if (m_entryIndexToContainerIndex.find(entryIndex) != m_entryIndexToContainerIndex.end()) {
        duk::log::warn("System of same type already exists, skipping");
        return;
    }
    m_container.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    m_entryIndexToContainerIndex.emplace(entryIndex, containerIndex);
}

template<typename T>
T* Systems::get() {
    static const auto entryIndex = SystemRegistry::index<T>();
    auto it = m_entryIndexToContainerIndex.find(entryIndex);
    if (it == m_entryIndexToContainerIndex.end()) {
        return nullptr;
    }
    const auto containerIndex = it.second;
    return dynamic_cast<T*>(m_container.at(containerIndex).get());
}

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance(true)->register_system<T>(std::forward<Args>(args)...);
}

void build_system(const std::string& entryName, Systems& systems);

}// namespace duk::scene

namespace duk::json {

template<>
inline void from_json<duk::scene::Systems>(const rapidjson::Value& jsonObject, duk::scene::Systems& systems) {
    auto jsonSystems = jsonObject.GetArray();
    for (auto& jsonSystem: jsonSystems) {
        const char* systemName = from_json<const char*>(jsonSystem);
        duk::scene::build_system(systemName, systems);
    }
}

}// namespace duk::json

#endif// DUK_SCENE_SYSTEMS_H
