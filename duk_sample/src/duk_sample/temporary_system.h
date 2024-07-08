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

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::sample::Temporary& temporary) {
    from_json_member(json, "duration", temporary.duration);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::sample::Temporary& temporary) {
    to_json_member(document, json, "duration", temporary.duration);
}

}// namespace duk::serial

#endif//DUK_SAMPLE_TEMPORARY_SYSTEM_H
