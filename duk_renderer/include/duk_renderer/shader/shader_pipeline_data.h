//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_DATA_H
#define DUK_RENDERER_SHADER_PIPELINE_DATA_H

#include <duk_serial/json/types.h>

#include <filesystem>

namespace duk::renderer {

struct ShaderPaths {
    std::filesystem::path vertexPath;
    std::filesystem::path fragmentPath;
};

struct PipelineSettings {
    bool blend;
    bool depth;
    bool invertY;
    duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask;
    uint32_t priority;
};

struct ShaderPipelineData {
    ShaderPaths shader;
    PipelineSettings settings;
};

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::PipelineSettings& data) {
    from_json_member(json, "blend", data.blend);
    from_json_member(json, "depth", data.depth);
    from_json_member(json, "invertY", data.invertY);
    from_json_member(json, "cullModeMask", data.cullModeMask);
    from_json_member(json, "priority", data.priority);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::PipelineSettings& data) {
    to_json_member(document, json, "blend", data.blend);
    to_json_member(document, json, "depth", data.depth);
    to_json_member(document, json, "invertY", data.invertY);
    to_json_member(document, json, "cullModeMask", data.cullModeMask);
    to_json_member(document, json, "priority", data.priority);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::ShaderPaths& data) {
    from_json_member(json, "vert", data.vertexPath);
    from_json_member(json, "frag", data.fragmentPath);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::ShaderPaths& data) {
    to_json_member(document, json, "vert", data.vertexPath);
    to_json_member(document, json, "frag", data.fragmentPath);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::ShaderPipelineData& data) {
    from_json_member(json, "shader", data.shader);
    from_json_member(json, "settings", data.settings);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::ShaderPipelineData& data) {
    to_json_member(document, json, "shader", data.shader);
    to_json_member(document, json, "settings", data.settings);
}

}// namespace duk::serial

#endif//DUK_RENDERER_SHADER_PIPELINE_DATA_H
