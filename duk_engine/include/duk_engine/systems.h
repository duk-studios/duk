/// 21/02/2024
/// system.h

#ifndef DUK_ENGINE_SYSTEMS_H
#define DUK_ENGINE_SYSTEMS_H

#include <duk_objects/objects.h>
#include <duk_tools/types.h>

namespace duk::engine {

class Engine;

class System {
public:
    virtual ~System() = default;

    virtual void enter(duk::objects::Objects& objects, Engine& engine) = 0;

    virtual void update(duk::objects::Objects& objects, Engine& engine) = 0;

    virtual void exit(duk::objects::Objects& objects, Engine& engine) = 0;
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
    template<bool isConst>
    class SystemIterator {
    public:
        using SystemsType = duk::tools::maybe_const_t<Systems, isConst>;
        using SystemType = duk::tools::maybe_const_t<System, isConst>;

        SystemIterator(SystemsType& systems, size_t i);

        SystemIterator& operator++();

        SystemIterator operator*() const;

        SystemType* operator->();

        SystemType* operator->() const;

        DUK_NO_DISCARD bool operator==(const SystemIterator& rhs) const;

        DUK_NO_DISCARD bool operator!=(const SystemIterator& rhs) const;

        DUK_NO_DISCARD size_t container_index() const;

        DUK_NO_DISCARD size_t system_index() const;

        DUK_NO_DISCARD const std::string& system_name() const;

    private:
        SystemsType& m_systems;
        size_t m_i;
    };

public:
    template<typename T>
    void add();

    template<typename T>
    T* get();

    void enter(duk::objects::Objects& objects, Engine& engine);

    void update(duk::objects::Objects& objects, Engine& engine);

    void exit(duk::objects::Objects& objects, Engine& engine);

    SystemIterator<false> begin();

    SystemIterator<false> end();

    SystemIterator<true> begin() const;

    SystemIterator<true> end() const;

private:
    DUK_NO_DISCARD size_t system_index(size_t containerIndex) const;

private:
    std::vector<std::unique_ptr<System>> m_container;
    std::unordered_map<size_t, size_t> m_systemIndexToContainerIndex;
    std::unordered_map<size_t, size_t> m_containerIndexToSystemIndex;
};

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance()->add<T>(std::forward<Args>(args)...);
}

void build_system(const std::string& systemName, Systems& systems);

const std::string& system_name(size_t systemIndex);

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

template<bool isConst>
Systems::SystemIterator<isConst>::SystemIterator(SystemsType& systems, size_t i)
    : m_systems(systems)
    , m_i(i) {
}

template<bool isConst>
Systems::SystemIterator<isConst>& Systems::SystemIterator<isConst>::operator++() {
    ++m_i;
    return *this;
}

template<bool isConst>
Systems::SystemIterator<isConst> Systems::SystemIterator<isConst>::operator*() const {
    return *this;
}

template<bool isConst>
typename Systems::SystemIterator<isConst>::SystemType* Systems::SystemIterator<isConst>::operator->() {
    return m_systems.m_container.at(m_i).get();
}

template<bool isConst>
typename Systems::SystemIterator<isConst>::SystemType* Systems::SystemIterator<isConst>::operator->() const {
    return m_systems.m_container.at(m_i).get();
}

template<bool isConst>
bool Systems::SystemIterator<isConst>::operator==(const SystemIterator& rhs) const {
    return &m_systems == &rhs.m_systems && m_i == rhs.m_i;
}

template<bool isConst>
bool Systems::SystemIterator<isConst>::operator!=(const SystemIterator& rhs) const {
    return !(*this == rhs);
}

template<bool isConst>
size_t Systems::SystemIterator<isConst>::container_index() const {
    return m_i;
}

template<bool isConst>
size_t Systems::SystemIterator<isConst>::system_index() const {
    return m_systems.system_index(m_i);
}

template<bool isConst>
const std::string& Systems::SystemIterator<isConst>::system_name() const {
    return duk::engine::system_name(system_index());
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

}// namespace duk::engine

namespace duk::serial {

template<>
inline void from_json<duk::engine::Systems>(const rapidjson::Value& json, duk::engine::Systems& systems) {
    auto jsonSystemsArray = json.GetArray();
    for (auto& jsonSystem: jsonSystemsArray) {
        std::string systemName;
        from_json(jsonSystem, systemName);
        duk::engine::build_system(systemName, systems);
    }
}

template<>
inline void to_json<duk::engine::Systems>(rapidjson::Document& document, rapidjson::Value& json, const duk::engine::Systems& systems) {
    auto jsonSystemsArray = json.SetArray().GetArray();
    for (auto it: systems) {
        const auto& systemName = it.system_name();
        rapidjson::Value jsonSystem;
        jsonSystem.SetString(systemName.c_str(), systemName.size(), document.GetAllocator());
        jsonSystemsArray.PushBack(std::move(jsonSystem), document.GetAllocator());
    }
}

}// namespace duk::serial

#endif// DUK_ENGINE_SYSTEMS_H
