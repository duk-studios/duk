//
// Created by Ricardo on 15/04/2023.
//

#include <duk_events/event.h>

namespace duk::events {

EventListener::EventListener() {
    static Id idCounter = 0;
    m_id = idCounter++;
}

EventListener::~EventListener() {
    for (auto event : m_events) {
        event->unsubscribe(m_id);
    }
}
}