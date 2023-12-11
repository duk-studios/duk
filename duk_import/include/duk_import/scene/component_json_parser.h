//
// Created by rov on 12/9/2023.
//

#ifndef DUK_IMPORT_COMPONENT_JSON_PARSER_H
#define DUK_IMPORT_COMPONENT_JSON_PARSER_H

#include <duk_import/json/types.h>
#include <duk_renderer/components/camera.h>

namespace duk::import {

struct ComponentParserCreateInfo {
    duk::renderer::MeshPool* meshPool;
    duk::renderer::ImagePool* imagePool;
    duk::renderer::MaterialPool* materialPool;
};

class ComponentJsonParser {
public:

    using ComponentBuilderPredicate = std::function<void(duk::scene::Object& object, const rapidjson::Value& jsonObject)>;

public:

    explicit ComponentJsonParser(const ComponentParserCreateInfo& componentParserCreateInfo);

    template<typename T>
    T to_component(const rapidjson::Value&) {
        return {};
    }

    template<typename T>
    void register_component(const std::string& name) {
        auto predicate = [this](duk::scene::Object& object, const rapidjson::Value& jsonObject) {
            *object.add<T>() = this->to_component<T>(jsonObject);
        };

        m_componentBuilders.emplace(name, predicate);
    }

    void parse(duk::scene::Object& object, const rapidjson::Value& member);

private:
    duk::renderer::MeshPool* m_meshPool;
    duk::renderer::ImagePool* m_imagePool;
    duk::renderer::MaterialPool* m_materialPool;
    std::unordered_map<std::string, ComponentBuilderPredicate> m_componentBuilders;
};


template<>
inline duk::renderer::Position3D
ComponentJsonParser::to_component<duk::renderer::Position3D>(const rapidjson::Value& member) {
    return {json::to_vec3(member["value"])};
}

template<>
inline duk::renderer::Position2D
ComponentJsonParser::to_component<duk::renderer::Position2D>(const rapidjson::Value& member) {
    return {json::to_vec2(member["value"])};
}

template<>
inline duk::renderer::Rotation3D
ComponentJsonParser::to_component<duk::renderer::Rotation3D>(const rapidjson::Value& member) {
    return {glm::quat(glm::radians(json::to_vec3(member["value"])))};
}

template<>
inline duk::renderer::Rotation2D
ComponentJsonParser::to_component<duk::renderer::Rotation2D>(const rapidjson::Value& member) {
    return {member["value"].GetFloat()};
}

template<>
inline duk::renderer::Scale2D
ComponentJsonParser::to_component<duk::renderer::Scale2D>(const rapidjson::Value& member) {
    return {json::to_vec2(member["value"])};
}

template<>
inline duk::renderer::Scale3D
ComponentJsonParser::to_component<duk::renderer::Scale3D>(const rapidjson::Value& member) {
    return {json::to_vec3(member["value"])};
}

template<>
inline duk::renderer::DirectionalLight
ComponentJsonParser::to_component<duk::renderer::DirectionalLight>(const rapidjson::Value& member) {
    duk::renderer::DirectionalLight directionalLight = {};
    directionalLight.value = json::to_light_value(member["value"]);
    return directionalLight;
}

template<>
inline duk::renderer::PointLight
ComponentJsonParser::to_component<duk::renderer::PointLight>(const rapidjson::Value& member) {
    duk::renderer::PointLight pointLight = {};
    pointLight.value = json::to_light_value(member["value"]);
    pointLight.radius = member["radius"].GetFloat();
    return pointLight;
}

template<>
inline duk::renderer::PerspectiveCamera
ComponentJsonParser::to_component<duk::renderer::PerspectiveCamera>(const rapidjson::Value& member) {
    duk::renderer::PerspectiveCamera perspectiveCamera = {};
    perspectiveCamera.fovDegrees = member["fov"].GetFloat();
    perspectiveCamera.aspectRatio = member["aspectRatio"].GetFloat();
    perspectiveCamera.zNear = member["near"].GetFloat();
    perspectiveCamera.zFar = member["far"].GetFloat();
    return perspectiveCamera;
}

template<>
inline duk::renderer::MeshDrawing
ComponentJsonParser::to_component<duk::renderer::MeshDrawing>(const rapidjson::Value& member) {
    duk::renderer::MeshDrawing meshDrawing = {};
    meshDrawing.mesh = m_meshPool->find(json::to_resource_id(member["mesh"]));
    meshDrawing.material = m_materialPool->find(json::to_resource_id(member["material"]));
    return meshDrawing;
}

}

#endif //DUK_IMPORT_COMPONENT_JSON_PARSER_H
