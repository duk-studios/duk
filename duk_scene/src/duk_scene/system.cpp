/// 21/02/2024
/// system.cpp

#include <duk_scene/systems.h>

namespace duk::scene {

size_t SystemRegistry::s_entryCounter = 0;

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

const std::string& SystemRegistry::system_name(size_t systemIndex) const {
    return m_systemEntries.at(systemIndex)->name();
}

System::~System() {
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
    return duk::scene::system_name(system_index());
}

Systems::SystemIterator Systems::begin() {
    return Systems::SystemIterator(*this, 0);
}

Systems::SystemIterator Systems::end() {
    return Systems::SystemIterator(*this, m_container.size());
}

void Systems::enter() {
    for (auto& system: m_container) {
        system->enter();
    }
}

void Systems::update() {
    for (auto& system: m_container) {
        system->update();
    }
}

void Systems::exit() {
    for (auto& system: m_container) {
        system->exit();
    }
}

size_t Systems::system_index(size_t containerIndex) const {
    return m_containerIndexToSystemIndex.at(containerIndex);
}

void build_system(const std::string& systemName, Systems& systems) {
    SystemRegistry::instance(true)->build(systemName, systems);
}

const std::string& system_name(size_t systemIndex) {
    return SystemRegistry::instance()->system_name(systemIndex);
}

}// namespace duk::scene