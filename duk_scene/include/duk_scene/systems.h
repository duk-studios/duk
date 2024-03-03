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

        virtual const std::string& name() = 0;
    };

    template<typename T, typename... Args>
    class SystemEntryT : public SystemEntry {
    public:
        SystemEntryT(Args&&... args);

        void build(Systems& systems) override;

        const std::string& name() override;

    private:
        std::tuple<Args...> m_buildArgs;
    };

public:
    template<typename T>
    static size_t index();

    template<typename T, typename... Args>
    void register_system(Args&&... args);

    void build(const std::string& systemName, Systems& systems);

    DUK_NO_DISCARD const std::string& system_name(size_t systemIndex) const;

private:
    static size_t s_entryCounter;
    std::vector<std::unique_ptr<SystemEntry>> m_systemEntries;
    std::unordered_map<std::string, size_t> m_systemNameToIndex;
};

class Systems {
public:
    class SystemIterator {
    public:
        SystemIterator(Systems& systems, size_t i);

        SystemIterator& operator++();

        SystemIterator& operator*();

        System* operator->();

        DUK_NO_DISCARD bool operator==(const SystemIterator& rhs);

        DUK_NO_DISCARD bool operator!=(const SystemIterator& rhs);

        DUK_NO_DISCARD size_t container_index() const;

        DUK_NO_DISCARD size_t system_index() const;

        DUK_NO_DISCARD const std::string& system_name() const;

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
    DUK_NO_DISCARD size_t system_index(size_t containerIndex) const;

private:
    std::vector<std::unique_ptr<System>> m_container;
    std::unordered_map<size_t, size_t> m_systemIndexToContainerIndex;
    std::unordered_map<size_t, size_t> m_containerIndexToSystemIndex;
};

namespace detail {
template<typename T, typename... Args>
static void build_system(Systems& systems, Args&&... args) {
    systems.add<T>(std::forward<Args>(args)...);
}
}// namespace detail

template<typename T, typename... Args>
SystemRegistry::SystemEntryT<T, Args...>::SystemEntryT(Args&&... args)
    : m_buildArgs(std::forward<Args>(args)...) {
}

template<typename T, typename... Args>
void SystemRegistry::SystemEntryT<T, Args...>::build(Systems& systems) {
    std::apply(
            [&systems](Args&&... args) {
                systems.add<T>(std::forward<Args>(args)...);
            },
            m_buildArgs);
}

template<typename T, typename... Args>
const std::string& SystemRegistry::SystemEntryT<T, Args...>::name() {
    return duk::tools::type_name_of<T>();
}

template<typename T>
size_t SystemRegistry::index() {
    static const auto index = s_entryCounter++;
    return index;
}

template<typename T, typename... Args>
void SystemRegistry::register_system(Args&&... args) {
    const auto index = SystemRegistry::index<T>();
    if (m_systemEntries.size() <= index) {
        m_systemEntries.resize(index + 1);
    }

    auto& entry = m_systemEntries[index];
    if (!entry) {
        entry = std::make_unique<SystemEntryT<T, Args...>>(std::forward<Args>(args)...);
        m_systemNameToIndex.emplace(duk::tools::type_name_of<T>(), index);
    }
}

template<typename T, typename... Args>
void Systems::add(Args&&... args) {
    static const auto entryIndex = SystemRegistry::index<T>();
    const auto containerIndex = m_container.size();
    if (m_systemIndexToContainerIndex.find(entryIndex) != m_systemIndexToContainerIndex.end()) {
        duk::log::warn("System of same type already exists, skipping");
        return;
    }
    m_container.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    m_systemIndexToContainerIndex.emplace(entryIndex, containerIndex);
    m_containerIndexToSystemIndex.emplace(containerIndex, entryIndex);
}

template<typename T>
T* Systems::get() {
    static const auto entryIndex = SystemRegistry::index<T>();
    auto it = m_systemIndexToContainerIndex.find(entryIndex);
    if (it == m_systemIndexToContainerIndex.end()) {
        return nullptr;
    }
    const auto containerIndex = it.second;
    return dynamic_cast<T*>(m_container.at(containerIndex).get());
}

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance(true)->register_system<T>(std::forward<Args>(args)...);
}

void build_system(const std::string& systemName, Systems& systems);

const std::string& system_name(size_t systemIndex);

}// namespace duk::scene

namespace duk::serial {

template<>
inline void read_array(JsonReader* reader, duk::scene::Systems& systems, size_t size) {
    for (size_t i = 0; i < size; i++) {
        std::string systemName;
        reader->visit_member(systemName, i);
        duk::scene::build_system(systemName, systems);
    }
}

template<>
inline void write_array(JsonWriter* writer, duk::scene::Systems& systems) {
    std::vector<std::string> systemNames;
    for (auto system: systems) {
        auto systemName = system.system_name();
        writer->visit_member(systemName, MemberDescription(nullptr));
    }
}

}// namespace duk::serial

#endif// DUK_SCENE_SYSTEMS_H
