/// 21/02/2024
/// system.cpp

#include <duk_system/system.h>

namespace duk::system {

namespace detail {

SystemRegistry g_systemRegistry;

static bool is_group_enabled(uint32_t groupIndex, uint32_t disabledGroupMask) {
    const uint32_t groupMask = 1 << groupIndex;
    return (groupMask & disabledGroupMask) == 0;
}

}// namespace detail

void SystemRegistry::from_json(Systems& systems, const rapidjson::Value& json, const std::string& systemName) {
    auto it = m_systemNameToIndex.find(systemName);
    if (it == m_systemNameToIndex.end()) {
        duk::log::warn("System entry name \"{}\" not registered", systemName);
        return;
    }
    const auto index = it->second;
    auto& entry = m_systemEntries.at(index);
    entry->from_json(systems, json);
}

void SystemRegistry::to_json(const Systems& systems, rapidjson::Document& document, rapidjson::Value& json, const std::string& systemName) {
    auto it = m_systemNameToIndex.find(systemName);
    if (it == m_systemNameToIndex.end()) {
        duk::log::warn("System entry name \"{}\" not registered", systemName);
        return;
    }
    const auto index = it->second;
    auto& entry = m_systemEntries.at(index);
    entry->to_json(systems, document, json);
}

const std::string& SystemRegistry::system_name(size_t systemIndex) const {
    return m_systemEntries.at(systemIndex)->name();
}

SystemEventDispatcher::SystemEventDispatcher(duk::objects::ComponentEventDispatcher& componentDispatcher)
    : m_componentDispatcher(componentDispatcher) {
}

System::System()
    : m_globals(nullptr)
    , m_dispatcher(nullptr) {
}

System::~System() = default;

void System::attach(duk::tools::Globals& globals, duk::objects::Objects& objects, SystemEventDispatcher& dispatcher) {
    m_globals = &globals;
    m_objects = &objects;
    m_dispatcher = &dispatcher;
    attach();
}

void System::attach() {
}

void System::enter() {
}

void System::update() {
}

void System::exit() {
}

duk::tools::Globals* System::globals() const {
    return m_globals;
}

duk::objects::Objects* System::objects() const {
    return m_objects;
}

duk::objects::Object System::create_object() const {
    return m_objects->add_object();
}

duk::objects::Object System::create_object(const duk::objects::Id& parent) const {
    return m_objects->add_object(parent);
}

duk::objects::Object System::create_object(const duk::objects::ConstObject& object) const {
    return m_objects->copy_object(object);
}

duk::objects::Object System::create_object(const duk::objects::Id& parent, const duk::objects::ConstObject& object) const {
    return m_objects->copy_object(object, parent);
}

duk::objects::Object System::create_object(const duk::objects::Objects& objects) const {
    return m_objects->copy_objects(objects);
}

duk::objects::Object System::create_object(const duk::objects::Id& parent, const duk::objects::Objects& objects) const {
    return m_objects->copy_objects(objects, parent);
}

duk::objects::Object System::create_object(const duk::objects::ObjectsResource& objects) const {
    return create_object(*objects);
}

duk::objects::Object System::create_object(const duk::objects::Id& parent, const duk::objects::ObjectsResource& objects) const {
    return create_object(parent, *objects);
}

SystemRegistry* SystemRegistry::instance() {
    return &detail::g_systemRegistry;
}

uint32_t SystemRegistry::index_of(const std::string& systemName) {
    return m_systemNameToIndex.at(systemName);
}

Systems::SystemIterator<false> Systems::begin() {
    return SystemIterator<false>(*this, 0);
}

Systems::SystemIterator<false> Systems::end() {
    return SystemIterator<false>(*this, m_systemGroup.size());
}

Systems::SystemIterator<true> Systems::begin() const {
    return SystemIterator<true>(*this, 0);
}

Systems::SystemIterator<true> Systems::end() const {
    return SystemIterator<true>(*this, 0);
}

Systems::Systems() = default;

void Systems::attach(duk::tools::Globals& globals, duk::objects::Objects& objects, SystemEventDispatcher& dispatcher) {
    for (auto& [system, group]: m_systemGroup) {
        system->attach(globals, objects, dispatcher);
    }
}

void Systems::enter(uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->enter();
        }
    }
}

void Systems::update(uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->update();
        }
    }
}

void Systems::exit(uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->exit();
        }
    }
}

size_t Systems::system_index(size_t containerIndex) const {
    return m_containerIndexToSystemIndex.at(containerIndex);
}

}// namespace duk::system