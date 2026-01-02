//
// Created by Ricardo on 03/05/2024.
//

#ifndef DUK_RENDERER_MATERIAL_DATA_H
#define DUK_RENDERER_MATERIAL_DATA_H

#include <duk_renderer/shader/shader_pipeline.h>
#include <duk_renderer/texture.h>
#include <duk_renderer/image/image.h>
#include <duk_serial/json.h>

#include <vector>

namespace duk::renderer {

enum class BindingType {
    UNDEFINED,
    UNIFORM,
    INSTANCE,
    IMAGE_SAMPLER,
    IMAGE
};

struct BindingData {
    virtual ~BindingData() = default;
};

struct Binding {
    std::string name;
    BindingType type;
    std::shared_ptr<BindingData> data;
};

struct BufferBinding : BindingData {
    struct Member {
        union Data {
            int intValue;
            unsigned int uintValue;
            float floatValue;
            bool boolValue;
            glm::vec2 vec2Value;
            glm::vec3 vec3Value;
            glm::vec4 vec4Value;
        };

        enum class Type {
            UNDEFINED,
            INT,
            UINT,
            FLOAT,
            BOOL,
            VEC2,
            VEC3,
            VEC4,
        };

        Type type;
        std::string name;
        Data data;
    };

    std::vector<Member> members;
};

struct ImageBinding : BindingData {
    ImageResource image;
};

struct ImageSamplerBinding : BindingData {
    ImageResource image;
    duk::rhi::Sampler sampler;

    duk::rhi::Descriptor descriptor() const {
        return duk::rhi::Descriptor::image_sampler(image->handle(), duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler);
    }
};

struct MaterialData {
    ShaderPipelineResource shader;
    std::vector<Binding> bindings;
};

}// namespace duk::renderer

namespace duk::type {

template<>
struct Type<duk::renderer::BufferBinding> : Class<duk::renderer::BufferBinding,
    Member<"members", &duk::renderer::BufferBinding::members>> {
};

template<>
struct Type<duk::renderer::ImageSamplerBinding> : Class<duk::renderer::ImageSamplerBinding,
    Member<"image", &duk::renderer::ImageSamplerBinding::image>,
    Member<"sampler", &duk::renderer::ImageSamplerBinding::sampler>> {
};

template<>
struct Type<duk::renderer::ImageBinding> : Class<duk::renderer::ImageBinding,
    Member<"image", &duk::renderer::ImageBinding::image>> {
};

template<>
struct Type<duk::renderer::MaterialData> : Class<duk::renderer::MaterialData,
    Member<"shader", &duk::renderer::MaterialData::shader>,
    Member<"bindings", &duk::renderer::MaterialData::bindings>> {
};

}

namespace duk::serial {

template<>
struct JsonPrimitiveValue<renderer::BufferBinding::Member::Type> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const renderer::BufferBinding::Member::Type& value);

    static void read(const rapidjson::Value& json, renderer::BufferBinding::Member::Type& value);
};

template<>
struct JsonPrimitiveValue<renderer::BufferBinding::Member> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const renderer::BufferBinding::Member& value);

    static void read(const rapidjson::Value& json, renderer::BufferBinding::Member& value);
};

template<>
struct JsonPrimitiveValue<renderer::BindingType> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const renderer::BindingType& value);

    static void read(const rapidjson::Value& json, renderer::BindingType& value);
};

template<>
struct JsonPrimitiveValue<renderer::Binding> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const renderer::Binding& value);

    static void read(const rapidjson::Value& json, renderer::Binding& value);
};

}// namespace duk::serial

#endif//DUK_RENDERER_MATERIAL_DATA_H
