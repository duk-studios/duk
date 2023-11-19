//
// Created by rov on 11/17/2023.
//
#include <duk_engine/systems/system.h>

namespace duk::engine {

System::System(Engine &engine, std::string name) :
    m_engine(engine),
    m_name(std::move(name)) {

}

System::~System() = default;

const std::string& System::name() {
    return m_name;
}

Engine *System::engine() {
    return &m_engine;
}

Engine* System::engine() const {
    return &m_engine;
}

Systems::Iterator Systems::begin() {
    return m_container.begin();
}

Systems::ConstIterator Systems::begin() const {
    return m_container.begin();
}

Systems::Iterator Systems::end() {
    return m_container.end();
}

Systems::ConstIterator Systems::end() const {
    return m_container.end();
}

}
