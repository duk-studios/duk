//
// Created by Ricardo on 22/07/2024.
//

#ifndef DUK_OBJECTS_ID_H
#define DUK_OBJECTS_ID_H

#include <duk_macros/macros.h>

#include <duk_serial/json/types.h>

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

    bool operator==(const Id& rhs) const;

    bool operator!=(const Id& rhs) const;

private:
    uint32_t m_index;
    uint32_t m_version;
};

}// namespace duk::objects

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::objects::Id& id) {
    uint32_t index;
    from_json(json, index);
    id = duk::objects::Id(index, 0);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::Id& id) {
    to_json(document, json, id.index());
}

}// namespace duk::serial

#endif//DUK_OBJECTS_ID_H
