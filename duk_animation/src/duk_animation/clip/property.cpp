//
// Created by Ricardo on 25/04/2024.
//

#include <duk_animation/clip/property.h>

namespace duk::animation {

PropertyRegistry g_instance;

PropertyRegistry* PropertyRegistry::instance() {
    return &g_instance;
}

void PropertyRegistry::json_write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& property) const {
    json.SetObject();
    const auto& type = property->name();
    const auto entry = find_entry(type);
    entry->json_write(document, json, property);
    serial::json_write_member_value(document, json, "type", type);
}

void PropertyRegistry::json_read(const rapidjson::Value& json, std::unique_ptr<Property>& property) const {
    std::string type;
    duk::serial::json_read_member_value(json, "type", type);
    const auto entry = find_entry(type);
    entry->json_read(json, property);
}

PropertyRegistry::PropertyEntry* PropertyRegistry::find_entry(const std::string& type) const {
    auto it = m_propertyEntries.find(type);
    if (it == m_propertyEntries.end()) {
        throw std::runtime_error(fmt::format("Unregisted animation property type '{}'", type));
    }
    return it->second.get();
}
}// namespace duk::animation

namespace duk::serial {

using namespace duk::animation;

void JsonPrimitiveValue<std::unique_ptr<Property>>::write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& value) {
    PropertyRegistry::instance()->json_write(document, json, value);
}

void JsonPrimitiveValue<std::unique_ptr<Property>>::read(const rapidjson::Value& json, std::unique_ptr<duk::animation::Property>& value) {
    PropertyRegistry::instance()->json_read(json, value);
}

}
