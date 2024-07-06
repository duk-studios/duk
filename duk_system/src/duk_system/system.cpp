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

}

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

Systems::Systems()
    : m_dispatcher(nullptr) {
}

void Systems::attach(duk::objects::ComponentEventDispatcher* dispatcher) {
    m_dispatcher = dispatcher;
    for (auto& [system, group]: m_systemGroup) {
        system->attach(dispatcher);
    }
}

void Systems::enter(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->enter(objects, globals);
        }
    }
}

void Systems::update(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->update(objects, globals);
        }
    }
}

void Systems::exit(duk::objects::Objects& objects, duk::tools::Globals& globals, uint32_t disabledGroupsMask) {
    for (auto& [system, group]: m_systemGroup) {
        if (detail::is_group_enabled(group, disabledGroupsMask)) {
            system->exit(objects, globals);
        }
    }
}

size_t Systems::system_index(size_t containerIndex) const {
    return m_containerIndexToSystemIndex.at(containerIndex);
}

}// namespace duk::engine