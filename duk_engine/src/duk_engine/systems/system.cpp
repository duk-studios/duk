//
// Created by rov on 11/17/2023.
//
#include <duk_engine/systems/system.h>

namespace duk::engine {

System::System(Engine& engine) :
    m_engine(engine) {

}

void System::enter() {
    enter(m_engine);
}

void System::update() {
    update(m_engine);
}

void System::exit() {
    exit(m_engine);
}

}
