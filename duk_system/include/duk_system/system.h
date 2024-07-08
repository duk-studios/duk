/// 21/02/2024
/// system.h

#ifndef DUK_SYSTEM_SYSTEM_H
#define DUK_SYSTEM_SYSTEM_H

#include <duk_objects/objects.h>
#include <duk_tools/globals.h>
#include <duk_tools/types.h>

namespace duk::system {

class SystemEventDispatcher {
public:
    SystemEventDispatcher(duk::objects::ComponentEventDispatcher& componentDispatcher);

    // emits an event E to component C for this object
    template<typename E, typename C>
    void emit_component(const duk::objects::Component<C>& component, const E& event);

    // emits an event E to component C for this object
    template<typename E, typename C>
    void emit_component(const duk::objects::Object& object, const E& event);

    // emits an event E to component C for all objects
    template<typename E, typename C>
    void emit_component(duk::objects::Objects& objects, const E& event);

    // emits an event E to all components for this object
    template<typename E>
    void emit_object(const duk::objects::Object& object, const E& event);

    template<typename E>
    void emit_global(const E& event);

    template<typename E, typename C, typename F>
    void listen_component(duk::event::Listener& listener, F&& callback);

    template<typename E, typename F>
    void listen_global(duk::event::Listener& listener, F&& callback);

private:
    duk::objects::ComponentEventDispatcher& m_componentDispatcher;
    duk::event::Dispatcher m_globalDispatcher;
};

class System {
public:
    System();

    virtual ~System();

    void attach(duk::tools::Globals& globals, duk::objects::Objects& objects, SystemEventDispatcher& dispatcher);

    virtual void attach();

    virtual void enter();

    virtual void update();

    virtual void exit();

    DUK_NO_DISCARD duk::tools::Globals* globals() const;

    template<typename T>
    DUK_NO_DISCARD T* global() const;

    DUK_NO_DISCARD duk::objects::Objects* objects() const;

    template<typename... Ts>
    DUK_NO_DISCARD auto all_objects_with() const;

    template<typename... Ts>
    DUK_NO_DISCARD auto first_object_with() const;

    template<typename... Ts>
    DUK_NO_DISCARD auto all_components_of() const;

    template<typename... Ts>
    DUK_NO_DISCARD auto first_components_of() const;

    DUK_NO_DISCARD duk::objects::Object create_object() const;

    DUK_NO_DISCARD duk::objects::Object create_object(const duk::objects::ConstObject& object) const;

    DUK_NO_DISCARD duk::objects::Object create_object(const duk::objects::Objects& objects) const;

    DUK_NO_DISCARD duk::objects::Object create_object(const duk::objects::ObjectsResource& objects) const;

    template<typename E>
    void emit_global(const E& event = {}) const;

    template<typename E>
    void emit_object(const duk::objects::Object& object, const E& event = {}) const;

    template<typename E, typename C>
    void emit_component(const duk::objects::Component<C>& component, const E& event = {}) const;

    template<typename E, typename C>
    void emit_component(const duk::objects::Object& object, const E& event = {}) const;

protected:
    template<typename E, typename C, typename R>
    void listen_component(R* receiver);

    template<typename TComponentEvent, typename R>
    void listen_component(R* receiver);

    template<typename E, typename R>
    void listen_global(R* receiver);

private:
    duk::tools::Globals* m_globals;
    duk::objects::Objects* m_objects;
    SystemEventDispatcher* m_dispatcher;
    duk::event::Listener m_listener;
};

class Systems;

class SystemRegistry {
private:
    class SystemEntry {
    public:
        virtual ~SystemEntry() = default;

        virtual void from_json(Systems& systems, const rapidjson::Value& json) = 0;

        virtual void to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json) = 0;

        virtual void solve(duk::resource::DependencySolver* dependencySolver, Systems& systems) = 0;

        virtual void solve(duk::resource::ReferenceSolver* referenceSolver, Systems& systems) = 0;

        virtual const std::string& name() = 0;
    };

    template<typename T>
    class SystemEntryT : public SystemEntry {
    public:
        void from_json(Systems& systems, const rapidjson::Value& json) override;

        void to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json) override;

        void solve(duk::resource::DependencySolver* solver, Systems& systems) override;

        void solve(duk::resource::ReferenceSolver* solver, Systems& systems) override;

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

    template<typename Solver>
    void solve_resources(Solver* solver, Systems& systems, const std::string& systemName);

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

    void attach(duk::tools::Globals& globals, duk::objects::Objects& objects, SystemEventDispatcher& dispatcher);

    template<typename T>
    T* add(uint32_t group);

    template<typename T>
    T* get() const;

    template<typename T>
    uint32_t group() const;

    void enter(uint32_t disabledGroupsMask);

    void update(uint32_t disabledGroupsMask);

    void exit(uint32_t disabledGroupsMask);

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
};

template<typename T, typename... Args>
void register_system(Args&&... args) {
    SystemRegistry::instance()->add<T>(std::forward<Args>(args)...);
}

template<typename E, typename C>
void SystemEventDispatcher::emit_component(const duk::objects::Component<C>& component, const E& event) {
    m_componentDispatcher.emit_component<E, C>(component, event);
}

template<typename E, typename C>
void SystemEventDispatcher::emit_component(const duk::objects::Object& object, const E& event) {
    m_componentDispatcher.emit_component<E, C>(object, event);
}

template<typename E, typename C>
void SystemEventDispatcher::emit_component(duk::objects::Objects& objects, const E& event) {
    for (auto object: objects.all_with<C>()) {
        emit_component<E, C>(object, event);
    }
}

template<typename E>
void SystemEventDispatcher::emit_object(const duk::objects::Object& object, const E& event) {
    m_componentDispatcher.emit_object<E>(object, event);
}

template<typename E>
void SystemEventDispatcher::emit_global(const E& event) {
    m_globalDispatcher.emit(event);
}

template<typename E, typename C, typename F>
void SystemEventDispatcher::listen_component(duk::event::Listener& listener, F&& callback) {
    m_componentDispatcher.listen<E, C>(listener, std::forward<F>(callback));
}

template<typename E, typename F>
void SystemEventDispatcher::listen_global(duk::event::Listener& listener, F&& callback) {
    m_globalDispatcher.add_listener<E>(listener, std::forward<F>(callback));
}

template<typename T>
T* System::global() const {
    return m_globals->get<T>();
}

template<typename... Ts>
auto System::all_objects_with() const {
    return m_objects->all_with<Ts...>();
}

template<typename... Ts>
auto System::first_object_with() const {
    return m_objects->first_with<Ts...>();
}

template<typename... Ts>
auto System::all_components_of() const {
    return m_objects->all_of<Ts...>();
}

template<typename... Ts>
auto System::first_components_of() const {
    return m_objects->first_of<Ts...>();
}

template<typename E>
void System::emit_global(const E& event) const {
    m_dispatcher->emit_global<E>(event);
}

template<typename E>
void System::emit_object(const duk::objects::Object& object, const E& event) const {
    m_dispatcher->emit_object<E>(object, event);
}

template<typename E, typename C>
void System::emit_component(const duk::objects::Component<C>& component, const E& event) const {
    m_dispatcher->emit_component<E, C>(component, event);
}

template<typename E, typename C>
void System::emit_component(const duk::objects::Object& object, const E& event) const {
    m_dispatcher->emit_component<E, C>(object, event);
}

template<typename E, typename C, typename R>
void System::listen_component(R* receiver) {
    m_dispatcher->listen_component<E, C>(m_listener, [receiver](const duk::objects::ComponentEvent<C, E>& event) {
        receiver->receive(event);
    });
}

template<typename TComponentEvent, typename R>
void System::listen_component(R* receiver) {
    using C = typename TComponentEvent::ComponentType;
    using E = typename TComponentEvent::EventType;
    listen_component<E, C, R>(receiver);
}

template<typename E, typename R>
void System::listen_global(R* receiver) {
    m_dispatcher->listen_global<E>(m_listener, [receiver](const E& event) {
        receiver->receive(event);
    });
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
void SystemRegistry::SystemEntryT<T>::solve(duk::resource::DependencySolver* solver, Systems& systems) {
    solver->solve(*systems.get<T>());
}

template<typename T>
void SystemRegistry::SystemEntryT<T>::solve(duk::resource::ReferenceSolver* solver, Systems& systems) {
    solver->solve(*systems.get<T>());
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

template<typename Solver>
void SystemRegistry::solve_resources(Solver* solver, Systems& systems, const std::string& systemName) {
    auto it = m_systemNameToIndex.find(systemName);
    if (it == m_systemNameToIndex.end()) {
        duk::log::warn("System entry name \"{}\" not registered", systemName);
        return;
    }
    const auto index = it->second;
    auto& entry = m_systemEntries.at(index);
    entry->solve(solver, systems);
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
    return static_cast<T*>(systemGroup.system.get());
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
}// namespace duk::system

namespace duk::serial {

template<>
inline void from_json<duk::system::Systems>(const rapidjson::Value& json, duk::system::Systems& systems) {
    auto systemJsonArray = json.GetArray();
    for (auto& systemJson: systemJsonArray) {
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

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::system::Systems& systems) {
    for (auto it: systems) {
        const auto& systemName = it.system_name();
        duk::system::SystemRegistry::instance()->solve_resources(solver, systems, systemName);
    }
}

}// namespace duk::resource

#endif// DUK_SYSTEM_SYSTEM_H
