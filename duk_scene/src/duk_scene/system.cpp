/// 21/02/2024
/// system.cpp

#include <duk_scene/systems.h>

namespace duk::scene {

size_t SystemRegistry::s_entryCounter = 0;

void SystemRegistry::build(const std::string& entryName, Systems& systems) {
    auto it = m_entryNameToIndex.find(entryName);
    if (it == m_entryNameToIndex.end()) {
        duk::log::warn("System entry name \"{}\" not registered", entryName);
        return;
    }
    const auto index = it->second;
    auto& entry = m_entries.at(index);
    entry->build(systems);
}

System::~System() {
}

Systems::SystemIterator::SystemIterator(Systems& systems, size_t i) : m_systems(systems),
                                                                      m_i(i) {
}

Systems::SystemIterator& Systems::SystemIterator::operator++() {
    ++m_i;
    return *this;
}

System* Systems::SystemIterator::operator*() {
    return m_systems.m_container.at(m_i).get();
}

bool Systems::SystemIterator::operator==(const Systems::SystemIterator& rhs) {
    return &m_systems == &rhs.m_systems && m_i == rhs.m_i;
}

bool Systems::SystemIterator::operator!=(const Systems::SystemIterator& rhs) {
    return !(*this == rhs);
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

void build_system(const std::string& entryName, Systems& systems) {
    SystemRegistry::instance(true)->build(entryName, systems);
}

}// namespace duk::scene