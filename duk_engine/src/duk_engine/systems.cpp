/// 21/02/2024
/// system.cpp

#include <duk_engine/systems.h>

namespace duk::engine {

namespace detail {

SystemRegistry g_systemRegistry;

}

System::System(uint32_t groupMask)
    : m_groupMask(groupMask) {
}

uint32_t System::group_mask() {
    return m_groupMask;
}

bool System::belongs(uint32_t groupMask) {
    return (m_groupMask & groupMask) == m_groupMask;
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

void SystemRegistry::build(const std::string& systemName, Systems& systems) {
    auto it = m_systemNameToIndex.find(systemName);
    if (it == m_systemNameToIndex.end()) {
        duk::log::warn("System entry name \"{}\" not registered", systemName);
        return;
    }
    const auto index = it->second;
    auto& entry = m_systemEntries.at(index);
    entry->build(systems);
}

Systems::SystemIterator::SystemIterator(Systems& systems, size_t i)
    : m_systems(systems)
    , m_i(i) {
}

Systems::SystemIterator& Systems::SystemIterator::operator++() {
    ++m_i;
    return *this;
}

Systems::SystemIterator& Systems::SystemIterator::operator*() {
    return *this;
}

System* Systems::SystemIterator::operator->() {
    return m_systems.m_container.at(m_i).get();
}

bool Systems::SystemIterator::operator==(const Systems::SystemIterator& rhs) {
    return &m_systems == &rhs.m_systems && m_i == rhs.m_i;
}

bool Systems::SystemIterator::operator!=(const Systems::SystemIterator& rhs) {
    return !(*this == rhs);
}

size_t Systems::SystemIterator::container_index() const {
    return m_i;
}

size_t Systems::SystemIterator::system_index() const {
    return m_systems.system_index(m_i);
}

const std::string& Systems::SystemIterator::system_name() const {
    return duk::engine::system_name(system_index());
}

Systems::SystemIterator Systems::begin() {
    return Systems::SystemIterator(*this, 0);
}

Systems::SystemIterator Systems::end() {
    return Systems::SystemIterator(*this, m_container.size());
}

void Systems::enter(duk::objects::Objects& objects, Engine& engine) {
    for (auto& system: m_container) {
        system->enter(objects, engine);
    }
}

void Systems::update(duk::objects::Objects& objects, Engine& engine, uint32_t activeGroupMask) {
    for (auto& system: m_container) {
        if (!system->belongs(activeGroupMask)) {
            continue;
        }

        system->update(objects, engine);
        objects.update();
    }
}

void Systems::exit(duk::objects::Objects& objects, Engine& engine) {
    for (auto& system: m_container) {
        system->exit(objects, engine);
    }
}

size_t Systems::system_index(size_t containerIndex) const {
    return m_containerIndexToSystemIndex.at(containerIndex);
}

void build_system(const std::string& systemName, Systems& systems) {
    SystemRegistry::instance()->build(systemName, systems);
}

const std::string& system_name(size_t systemIndex) {
    return SystemRegistry::instance()->system_name(systemIndex);
}

}// namespace duk::engine