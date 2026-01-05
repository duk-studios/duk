//
// Created by Ricardo on 05/07/2024.
//

#ifndef DUK_SAMPLE_TEMPORARY_SYSTEM_H
#define DUK_SAMPLE_TEMPORARY_SYSTEM_H

#include <duk_system/system.h>

#include <duk_objects/events.h>

namespace duk::sample {

struct Temporary {
    float duration;
    float startTime = 0.0f;
};

class TemporarySystem : public duk::system::System {
public:
    using TemporaryEnterEvent = duk::objects::ComponentEvent<Temporary, duk::objects::ComponentEnterEvent>;

    void attach() override;

    void update() override;

    void receive(const TemporaryEnterEvent& event);
};

}// namespace duk::sample

namespace duk::type {

template<>
struct Type<duk::sample::Temporary> : Class<duk::sample::Temporary,
    Member<"duration", &duk::sample::Temporary::duration>,
    Member<"startTime", &duk::sample::Temporary::startTime>> {
};

}// namespace duk::type

#endif//DUK_SAMPLE_TEMPORARY_SYSTEM_H
