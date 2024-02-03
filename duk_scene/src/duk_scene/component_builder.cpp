/// 03/02/2024
/// component_registry.cpp

#include <duk_scene/component_builder.h>
#include <duk_json/string.h>
#include <duk_log/log.h>

namespace duk::scene {

void ComponentBuilder::build(Object& object, const rapidjson::Value& jsonObject) {

    auto typeIt = jsonObject.FindMember("type");
    if (typeIt == jsonObject.MemberEnd()) {
        duk::log::warn("Missing \"type\" field on component json: {}", duk::json::to_string(jsonObject));
        return;
    }

    std::string typeName = typeIt->value.GetString();

    auto registryEntry = m_componentBuilders.find(typeName);

    if (registryEntry == m_componentBuilders.end()) {
        duk::log::warn("Component \"{}\" not found in ComponentBuilder", typeName);
        return;
    }

    auto& builder = registryEntry->second;

    builder->build(object, jsonObject);
}

}