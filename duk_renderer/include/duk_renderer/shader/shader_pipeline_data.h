//
// Created by Ricardo on 09/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_DATA_H
#define DUK_RENDERER_SHADER_PIPELINE_DATA_H

#include <duk_renderer/shader/shader_module.h>

#include <duk_serial/json/types.h>

namespace duk::renderer {

struct PipelineShaderModules {
    ShaderModuleResource vert;
    ShaderModuleResource frag;
};

struct PipelineSettings {
    bool blend;
    bool depth;
    bool invertY;
    duk::rhi::GraphicsPipeline::CullMode::Mask cullModeMask;
    uint32_t priority;
};

struct ShaderPipelineData {
    PipelineShaderModules shaders;
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
inline void from_json(const rapidjson::Value& json, duk::renderer::PipelineShaderModules& shaders) {
    from_json_member(json, "vert", shaders.vert);
    from_json_member(json, "frag", shaders.frag);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::PipelineShaderModules& shaders) {
    to_json_member(document, json, "vert", shaders.vert);
    to_json_member(document, json, "frag", shaders.frag);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::ShaderPipelineData& data) {
    from_json_member(json, "shaders", data.shaders);
    from_json_member(json, "settings", data.settings);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::ShaderPipelineData& data) {
    to_json_member(document, json, "shaders", data.shaders);
    to_json_member(document, json, "settings", data.settings);
}

}// namespace duk::serial

#endif//DUK_RENDERER_SHADER_PIPELINE_DATA_H
