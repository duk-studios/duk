//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/clip/property.h>

#include <duk_serial/json/serializer.h>

namespace duk::animation {

PropertyRegistry g_instance;

PropertyRegistry* PropertyRegistry::instance() {
    return &g_instance;
}

void PropertyRegistry::from_json(const rapidjson::Value& json, std::unique_ptr<Property>& property) const {
    std::string type;
    duk::serial::from_json_member(json, "type", type);
    const auto entry = find_entry(type);
    entry->from_json(json, property);
}

PropertyRegistry::PropertyEntry* PropertyRegistry::find_entry(const std::string& type) const {
    auto it = m_propertyEntries.find(type);
    if (it == m_propertyEntries.end()) {
        throw std::runtime_error(fmt::format("Unregisted animation property type '{}'", type));
    }
    return it->second.get();
}
}// namespace duk::animation
