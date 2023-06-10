//
// Created by Ricardo on 15/04/2023.
//

#include <duk_events/event.h>

namespace duk::events {

Event::Handle::Handle(Event& owner) :
        m_owner(owner) {
    m_controlBlock = m_owner.m_controlBlock;
}

void Event::Handle::unsubscribe(size_t id) {
    if (m_controlBlock.lock()){
        m_owner.unsubscribe(id);
    }
}

Event::Handle& Event::Handle::operator=(const Event::Handle& other) {
    if (other == *this) {
        return *this;
    }
    m_owner = other.m_owner;
    m_controlBlock = other.m_controlBlock;
    return *this;
}


bool Event::Handle::operator==(Event::Handle& other) const noexcept {
    return &m_owner == &other.m_owner;
}

Event::Event() {
    // we just need something to hold the ref count
    m_controlBlock = std::make_shared<uint8_t>();
}

Event::~Event() = default;

EventListener::EventListener() {
    static size_t idCounter = 0;
    m_id = idCounter++;
}

EventListener::~EventListener() {
    for (auto& event : m_events) {
        event.unsubscribe(m_id);
    }
}

}
