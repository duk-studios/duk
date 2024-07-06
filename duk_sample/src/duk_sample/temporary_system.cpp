//
// Created by Ricardo on 05/07/2024.
//

#include <duk_sample/temporary_system.h>

#include <duk_tools/timer.h>

namespace duk::sample {

void TemporarySystem::enter(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    listen<Temporary, duk::objects::ComponentEnterEvent>(this);
}

void TemporarySystem::update(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    auto time = globals.get<duk::tools::Timer>()->time();
    for (auto [temporary] : objects.all_of<Temporary>()) {
        if (time > temporary->startTime + temporary->duration) {
            temporary.object().destroy();
        }
    }
}

void TemporarySystem::exit(duk::objects::Objects& objects, duk::tools::Globals& globals) {
}

void TemporarySystem::receive(const duk::objects::ComponentEvent<Temporary, duk::objects::ComponentEnterEvent>& event) {
    event.component->startTime = event.globals.get<duk::tools::Timer>()->time();
}

} // duk