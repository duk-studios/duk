//
// Created by Ricardo on 03/05/2024.
//

#ifndef DUK_RENDERER_MATERIAL_DATA_H
#define DUK_RENDERER_MATERIAL_DATA_H

#include <duk_renderer/shader/shader_pipeline.h>
#include <duk_renderer/texture.h>
#include <duk_renderer/image/image.h>
#include <duk_serial/json/types.h>

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

    duk::rhi::Descriptor descriptor() {
        return duk::rhi::Descriptor::image_sampler(image->handle(), duk::rhi::Image::Layout::SHADER_READ_ONLY, sampler);
    }
};

struct MaterialData {
    ShaderPipelineResource shader;
    std::vector<Binding> bindings;
};

}

namespace duk::serial {

namespace detail {

static duk::renderer::BindingType parse_material_data_binding_type(const std::string_view& str) {
    if (str == "uniform") {
        return duk::renderer::BindingType::UNIFORM;
    }
    if (str == "instance") {
        return duk::renderer::BindingType::INSTANCE;
    }
    if (str == "image-sampler") {
        return duk::renderer::BindingType::IMAGE_SAMPLER;
    }
    if (str == "image") {
        return duk::renderer::BindingType::IMAGE;
    }
    return duk::renderer::BindingType::UNDEFINED;
}

static duk::renderer::BufferBinding::Member::Type parse_material_data_binding_buffer_member_type(const std::string_view& str) {
    if (str == "int") {
        return duk::renderer::BufferBinding::Member::Type::INT;
    }
    if (str == "uint") {
        return duk::renderer::BufferBinding::Member::Type::UINT;
    }
    if (str == "float") {
        return duk::renderer::BufferBinding::Member::Type::FLOAT;
    }
    if (str == "bool") {
        return duk::renderer::BufferBinding::Member::Type::BOOL;
    }
    if (str == "vec2") {
        return duk::renderer::BufferBinding::Member::Type::VEC2;
    }
    if (str == "vec3") {
        return duk::renderer::BufferBinding::Member::Type::VEC3;
    }
    if (str == "vec4") {
        return duk::renderer::BufferBinding::Member::Type::VEC4;
    }
    return duk::renderer::BufferBinding::Member::Type::UNDEFINED;
}

}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::BufferBinding::Member::Type& type) {
    type = detail::parse_material_data_binding_buffer_member_type(json.GetString());
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::BufferBinding::Member& member) {
    from_json_member(json, "type", member.type);
    from_json_member(json, "name", member.name);
    switch (member.type) {
        case renderer::BufferBinding::Member::Type::INT:
            from_json_member(json, "value", member.data.intValue);
            break;
        case renderer::BufferBinding::Member::Type::UINT:
            from_json_member(json, "value", member.data.uintValue);
            break;
        case renderer::BufferBinding::Member::Type::FLOAT:
            from_json_member(json, "value", member.data.floatValue);
            break;
        case renderer::BufferBinding::Member::Type::BOOL:
            from_json_member(json, "value", member.data.boolValue);
            break;
        case renderer::BufferBinding::Member::Type::VEC2:
            from_json_member(json, "value", member.data.vec2Value);
            break;
        case renderer::BufferBinding::Member::Type::VEC3:
            from_json_member(json, "value", member.data.vec3Value);
            break;
        case renderer::BufferBinding::Member::Type::VEC4:
            from_json_member(json, "value", member.data.vec4Value);
            break;
        default:
            throw std::runtime_error("Unknown buffer member type");
    }
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::BindingType& bindingType) {
    bindingType = detail::parse_material_data_binding_type(json.GetString());
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::BufferBinding& buffer) {
    from_json_member(json, "members", buffer.members, true);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::ImageSamplerBinding& imageSampler) {
    from_json_member(json, "image", imageSampler.image);
    from_json_member(json, "sampler", imageSampler.sampler);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::ImageBinding& image) {
    from_json_member(json, "image", image.image);
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::Binding& binding) {
    from_json_member(json, "name", binding.name);
    from_json_member(json, "type", binding.type);
    switch (binding.type) {
        case renderer::BindingType::UNIFORM:
        case renderer::BindingType::INSTANCE: {
            auto bufferData = std::make_shared<duk::renderer::BufferBinding>();
            from_json(json, *bufferData);
            binding.data = std::move(bufferData);
            break;
        }
        case renderer::BindingType::IMAGE_SAMPLER: {
            auto textureData = std::make_shared<duk::renderer::ImageSamplerBinding>();
            from_json(json, *textureData);
            binding.data = std::move(textureData);
            break;
        }
        case renderer::BindingType::IMAGE: {
            auto imageData = std::make_shared<duk::renderer::ImageBinding>();
            from_json(json, *imageData);
            binding.data = std::move(imageData);
            break;
        }
        default:
            throw std::runtime_error("Unknown material binding type");
    }
}

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::MaterialData& materialData) {
    from_json_member(json, "shader", materialData.shader);
    from_json_member(json, "bindings", materialData.bindings);
}

}

#endif //DUK_RENDERER_MATERIAL_DATA_H
