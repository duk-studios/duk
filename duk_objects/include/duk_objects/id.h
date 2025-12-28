//
// Created by Ricardo on 22/07/2024.
//

#ifndef DUK_OBJECTS_ID_H
#define DUK_OBJECTS_ID_H

#include <duk_macros/macros.h>

#include <duk_serial/json.h>

#include <cstdint>
#include <limits>

namespace duk::objects {

static constexpr uint32_t kInvalidObjectIndex = std::numeric_limits<uint32_t>::max();

class Id {
public:
    Id();

    explicit Id(uint32_t index, uint32_t version);

    DUK_NO_DISCARD uint32_t index() const;

    DUK_NO_DISCARD uint32_t version() const;

    auto operator<=>(const Id& other) const = default;

private:
    uint32_t m_index;
    uint32_t m_version;
};

}// namespace duk::objects

namespace duk::serial {

template<>
struct JsonPrimitiveValue<duk::objects::Id> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::Id& id) {
        json.SetUint(id.index());
    }

    static void read(const rapidjson::Value& json, duk::objects::Id& id) {
        id = duk::objects::Id(json.GetUint(), 0);
    }
};

}// namespace duk::serial

#endif//DUK_OBJECTS_ID_H
