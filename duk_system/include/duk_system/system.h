/// 21/02/2024
/// system.h

#ifndef DUK_SYSTEM_SYSTEM_H
#define DUK_SYSTEM_SYSTEM_H

#include <duk_objects/objects.h>
#include <duk_tools/globals.h>
#include <duk_tools/types.h>

namespace duk::system {

class System : public duk::objects::ComponentEventListener {
public:
    virtual ~System() = default;

    virtual void enter(duk::objects::Objects& objects, duk::tools::Globals& globals) = 0;

    virtual void update(duk::objects::Objects& objects, duk::tools::Globals& globals) = 0;

    virtual void exit(duk::objects::Objects& objects, duk::tools::Globals& globals) = 0;
};

class Systems;

class SystemRegistry {
private:
    class SystemEntry {
    public:
        virtual ~SystemEntry() = default;

        virtual void from_json(Systems& systems, const rapidjson::Value& json) = 0;

        virtual void to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json) = 0;

        virtual const std::string& name() = 0;
    };

    template<typename T>
    class SystemEntryT : public SystemEntry {
    public:
        void from_json(Systems& systems, const rapidjson::Value& json) override;

        void to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json) override;

        const std::string& name() override;
    };

public:
    static SystemRegistry* instance();

    uint32_t index_of(const std::string& systemName);

    template<typename T>
    uint32_t index_of();

    template<typename T>
    void add();

    void from_json(Systems& systems, const rapidjson::Value& json, const std::string& systemName);

    void to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json, const std::string& systemName);

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

    Systems();

    void attach(duk::objects::ComponentEventDispatcher* dispatcher);

    template<typename T>
    T* add(uint32_t group);

    template<typename T>
    T* get() const;

    template<typename T>
    uint32_t group() const;

    void enter(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask);

    void update(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask);

    void exit(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask);

    SystemIterator<false> begin();

    SystemIterator<false> end();

    SystemIterator<true> begin() const;

    SystemIterator<true> end() const;

private:
    DUK_NO_DISCARD size_t system_index(size_t containerIndex) const;

private:

    struct SystemGroupEntry {
        std::unique_ptr<System> system;
        uint32_t group;
    };

    std::vector<SystemGroupEntry> m_systemGroup;
    std::unordered_map<size_t, size_t> m_systemIndexToContainerIndex;
    std::unordered_map<size_t, size_t> m_containerIndexToSystemIndex;
    duk::objects::ComponentEventDispatcher* m_dispatcher;
};

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance()->add<T>(std::forward<Args>(args)...);
}

template<typename T>
void SystemRegistry::SystemEntryT<T>::from_json(Systems& systems, const rapidjson::Value& json) {
    uint32_t group;
    duk::serial::from_json_member(json, "group", group);

    auto system = systems.add<T>(group);
    if (!system) {
        return;
    }
    duk::serial::from_json(json, *system);
}

template<typename T>
void SystemRegistry::SystemEntryT<T>::to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json) {
    duk::serial::to_json(document, json, *systems.get<T>());
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
    return m_systems.m_systems.at(m_i).get();
}

template<bool isConst>
typename Systems::SystemIterator<isConst>::SystemType* Systems::SystemIterator<isConst>::operator->() const {
    return m_systems.m_systems.at(m_i).get();
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
    return SystemRegistry::instance()->system_name(system_index());
}

template<typename T>
T* Systems::add(uint32_t group) {
    static const auto entryIndex = SystemRegistry::instance()->index_of<T>();
    const auto containerIndex = m_systemGroup.size();
    if (m_systemIndexToContainerIndex.contains(entryIndex)) {
        duk::log::warn("System of same type already exists, skipping");
        return nullptr;
    }
    auto& systemGroup = m_systemGroup.emplace_back(std::make_unique<T>(), group);
    m_systemIndexToContainerIndex.emplace(entryIndex, containerIndex);
    m_containerIndexToSystemIndex.emplace(containerIndex, entryIndex);
    auto system = static_cast<T*>(systemGroup.system.get());
    system->attach(m_dispatcher);
    return system;
}

template<typename T>
T* Systems::get() const {
    static const auto entryIndex = SystemRegistry::instance()->index_of<T>();
    auto it = m_systemIndexToContainerIndex.find(entryIndex);
    if (it == m_systemIndexToContainerIndex.end()) {
        return nullptr;
    }
    const auto containerIndex = it->second;
    return static_cast<T*>(m_systemGroup.at(containerIndex).system.get());
}

template<typename T>
uint32_t Systems::group() const {
    static const auto entryIndex = SystemRegistry::instance()->index_of<T>();
    auto it = m_systemIndexToContainerIndex.find(entryIndex);
    if (it == m_systemIndexToContainerIndex.end()) {
        return ~0;
    }
    const auto containerIndex = it->second;
    return m_systemGroup.at(containerIndex).group;

}
}// namespace duk::engine

namespace duk::serial {

template<>
inline void from_json<duk::system::Systems>(const rapidjson::Value& json, duk::system::Systems& systems) {
    auto systemJsonArray = json.GetArray();
    for (auto& systemJson : systemJsonArray) {
        std::string systemName;
        from_json_member(systemJson, "type", systemName);
        duk::system::SystemRegistry::instance()->from_json(systems, systemJson, systemName);
    }
}

template<>
inline void to_json<duk::system::Systems>(rapidjson::Document& document, rapidjson::Value& json, const duk::system::Systems& systems) {
    auto jsonSystemsArray = json.SetArray().GetArray();
    for (auto it: systems) {
        const auto& systemName = it.system_name();
        rapidjson::Value systemJson;
        to_json_member(document, systemJson, "type", systemName);

        duk::system::SystemRegistry::instance()->to_json(systems, document, systemJson, systemName);
    }
}

}// namespace duk::serial

#endif// DUK_SYSTEM_SYSTEM_H
