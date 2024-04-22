/// 21/02/2024
/// system.h

#ifndef DUK_ENGINE_SYSTEMS_H
#define DUK_ENGINE_SYSTEMS_H

#include <duk_objects/objects.h>
#include <duk_tools/types.h>

static constexpr uint32_t kMainThreadGroup = 1 << 0;
static constexpr uint32_t kGameplayGroup = 1 << 1;
static constexpr uint32_t kPhysicsThreadGroup = 1 << 2;

namespace duk::engine {

class Engine;

class System {
public:
    System(uint32_t groupMask);

    virtual ~System() = default;

    virtual void enter(duk::objects::Objects& objects, Engine& engine) = 0;

    virtual void update(duk::objects::Objects& objects, Engine& engine) = 0;

    virtual void exit(duk::objects::Objects& objects, Engine& engine) = 0;

    uint32_t group_mask() const;

    bool belongs(const uint32_t groupMask) const;

private:
    uint32_t m_groupMask;
};

class Systems;

class SystemRegistry {
private:
    class SystemEntry {
    public:
        virtual ~SystemEntry() = default;

        virtual void build(Systems& systems) = 0;

        virtual const std::string& name() = 0;
    };

    template<typename T>
    class SystemEntryT : public SystemEntry {
    public:
        void build(Systems& systems) override;

        const std::string& name() override;
    };

public:
    static SystemRegistry* instance();

    uint32_t index_of(const std::string& systemName);

    template<typename T>
    uint32_t index_of();

    template<typename T>
    void add();

    void build(const std::string& systemName, Systems& systems);

    DUK_NO_DISCARD const std::string& system_name(size_t systemIndex) const;

private:
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
    template<typename T>
    void add();

    template<typename T>
    T* get();

    void enter(duk::objects::Objects& objects, Engine& engine);

    void update(duk::objects::Objects& objects, Engine& engine, uint32_t activeGroupMask);

    void exit(duk::objects::Objects& objects, Engine& engine);

    SystemIterator begin();

    SystemIterator end();

private:
    DUK_NO_DISCARD size_t system_index(size_t containerIndex) const;

private:
    std::vector<std::unique_ptr<System>> m_container;
    std::unordered_map<size_t, size_t> m_systemIndexToContainerIndex;
    std::unordered_map<size_t, size_t> m_containerIndexToSystemIndex;
};

template<typename T>
void SystemRegistry::SystemEntryT<T>::build(Systems& systems) {
    systems.add<T>();
}

template<typename T>
const std::string& SystemRegistry::SystemEntryT<T>::name() {
    return duk::tools::type_name_of<T>();
}

template<typename T>
uint32_t SystemRegistry::index_of() {
    static const auto index = index_of(duk::tools::type_name_of<T>());
    return index;
}

template<typename T>
void SystemRegistry::add() {
    const auto& name = duk::tools::type_name_of<T>();
    auto it = m_systemNameToIndex.find(name);
    if (it != m_systemNameToIndex.end()) {
        return;
    }
    const auto index = m_systemEntries.size();
    m_systemEntries.emplace_back(std::make_unique<SystemEntryT<T>>());
    m_systemNameToIndex.emplace(name, index);
}

template<typename T>
void Systems::add() {
    static const auto entryIndex = SystemRegistry::instance()->index_of<T>();
    const auto containerIndex = m_container.size();
    if (m_systemIndexToContainerIndex.find(entryIndex) != m_systemIndexToContainerIndex.end()) {
        duk::log::warn("System of same type already exists, skipping");
        return;
    }
    m_container.emplace_back(std::make_unique<T>());
    m_systemIndexToContainerIndex.emplace(entryIndex, containerIndex);
    m_containerIndexToSystemIndex.emplace(containerIndex, entryIndex);
}

template<typename T>
T* Systems::get() {
    static const auto entryIndex = SystemRegistry::instance()->index_of<T>();
    auto it = m_systemIndexToContainerIndex.find(entryIndex);
    if (it == m_systemIndexToContainerIndex.end()) {
        return nullptr;
    }
    const auto containerIndex = it.second;
    return dynamic_cast<T*>(m_container.at(containerIndex).get());
}

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance()->add<T>(std::forward<Args>(args)...);
}

void build_system(const std::string& systemName, Systems& systems);

const std::string& system_name(size_t systemIndex);

}// namespace duk::engine

namespace duk::serial {

template<>
inline void read_array(JsonReader* reader, duk::engine::Systems& systems, size_t size) {
    for (size_t i = 0; i < size; i++) {
        std::string systemName;
        reader->visit_member(systemName, i);
        duk::engine::build_system(systemName, systems);
    }
}

template<>
inline void write_array(JsonWriter* writer, duk::engine::Systems& systems) {
    std::vector<std::string> systemNames;
    for (auto system: systems) {
        auto systemName = system.system_name();
        writer->visit_member(systemName, MemberDescription(nullptr));
    }
}

}// namespace duk::serial

#endif// DUK_ENGINE_SYSTEMS_H
