//
// Created by Ricardo on 03/05/2024.
//

#include <duk_renderer/material/material_data.h>

using namespace duk::serial;
using namespace duk::renderer;

namespace detail {

static BindingType parse_material_data_binding_type(const std::string_view& str) {
    if (str == "uniform") {
        return BindingType::UNIFORM;
    }
    if (str == "instance") {
        return BindingType::INSTANCE;
    }
    if (str == "image-sampler") {
        return BindingType::IMAGE_SAMPLER;
    }
    if (str == "image") {
        return BindingType::IMAGE;
    }
    return BindingType::UNDEFINED;
}

static std::string to_string(const BindingType& type) {
    switch (type) {
        case BindingType::UNIFORM:
            return "uniform";
        case BindingType::INSTANCE:
            return "instance";
        case BindingType::IMAGE_SAMPLER:
            return "image-sampler";
        case BindingType::IMAGE:
            return "image";
        default:
            return "undefined";
    }
}

static BufferBinding::Member::Type parse_material_data_binding_buffer_member_type(const std::string_view& str) {
    if (str == "int") {
        return BufferBinding::Member::Type::INT;
    }
    if (str == "uint") {
        return BufferBinding::Member::Type::UINT;
    }
    if (str == "float") {
        return BufferBinding::Member::Type::FLOAT;
    }
    if (str == "bool") {
        return BufferBinding::Member::Type::BOOL;
    }
    if (str == "vec2") {
        return BufferBinding::Member::Type::VEC2;
    }
    if (str == "vec3") {
        return BufferBinding::Member::Type::VEC3;
    }
    if (str == "vec4") {
        return BufferBinding::Member::Type::VEC4;
    }
    return BufferBinding::Member::Type::UNDEFINED;
}

static std::string to_string(const BufferBinding::Member::Type& type) {
    switch (type) {
        case BufferBinding::Member::Type::INT:
            return "int";
        case BufferBinding::Member::Type::UINT:
            return "uint";
        case BufferBinding::Member::Type::FLOAT:
            return "float";
        case BufferBinding::Member::Type::BOOL:
            return "bool";
        case BufferBinding::Member::Type::VEC2:
            return "vec2";
        case BufferBinding::Member::Type::VEC3:
            return "vec3";
        case BufferBinding::Member::Type::VEC4:
            return "vec4";
        default:
            return "undefined";
    }
}

}// namespace detail

void JsonPrimitiveValue<BufferBinding::Member::Type>::write(rapidjson::Document& document, rapidjson::Value& json, const BufferBinding::Member::Type& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<BufferBinding::Member::Type>::read(const rapidjson::Value& json, BufferBinding::Member::Type& value) {
    value = detail::parse_material_data_binding_buffer_member_type(json.GetString());
}

void JsonPrimitiveValue<BufferBinding::Member>::write(rapidjson::Document& document, rapidjson::Value& json, const BufferBinding::Member& value) {
    json_write_member_value(document, json, "type", value.type);
    json_write_member_value(document, json, "name", value.name);
    switch (value.type) {
        case BufferBinding::Member::Type::INT:
            json_write_member_value(document, json, "value", value.data.intValue);
            break;
        case BufferBinding::Member::Type::UINT:
            json_write_member_value(document, json, "value", value.data.uintValue);
            break;
        case BufferBinding::Member::Type::FLOAT:
            json_write_member_value(document, json, "value", value.data.floatValue);
            break;
        case BufferBinding::Member::Type::BOOL:
            json_write_member_value(document, json, "value", value.data.boolValue);
            break;
        case BufferBinding::Member::Type::VEC2:
            json_write_member_value(document, json, "value", value.data.vec2Value);
            break;
        case BufferBinding::Member::Type::VEC3:
            json_write_member_value(document, json, "value", value.data.vec3Value);
            break;
        case BufferBinding::Member::Type::VEC4:
            json_write_member_value(document, json, "value", value.data.vec4Value);
            break;
        default:
            throw std::runtime_error("Unknown buffer member type");
    }
}

void JsonPrimitiveValue<BufferBinding::Member>::read(const rapidjson::Value& json, BufferBinding::Member& value) {
    json_read_member_value(json, "type", value.type);
    json_read_member_value(json, "name", value.name);
    switch (value.type) {
        case BufferBinding::Member::Type::INT:
            json_read_member_value(json, "value", value.data.intValue);
            break;
        case BufferBinding::Member::Type::UINT:
            json_read_member_value(json, "value", value.data.uintValue);
            break;
        case BufferBinding::Member::Type::FLOAT:
            json_read_member_value(json, "value", value.data.floatValue);
            break;
        case BufferBinding::Member::Type::BOOL:
            json_read_member_value(json, "value", value.data.boolValue);
            break;
        case BufferBinding::Member::Type::VEC2:
            json_read_member_value(json, "value", value.data.vec2Value);
            break;
        case BufferBinding::Member::Type::VEC3:
            json_read_member_value(json, "value", value.data.vec3Value);
            break;
        case BufferBinding::Member::Type::VEC4:
            json_read_member_value(json, "value", value.data.vec4Value);
            break;
        default:
            throw std::runtime_error("Unknown buffer member type");
    }
}

void JsonPrimitiveValue<BindingType>::write(rapidjson::Document& document, rapidjson::Value& json, const BindingType& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<BindingType>::read(const rapidjson::Value& json, BindingType& value) {
    value = detail::parse_material_data_binding_type(json.GetString());
}

void JsonPrimitiveValue<Binding>::write(rapidjson::Document& document, rapidjson::Value& json, const Binding& value) {
    json_write_member_value(document, json, "type", value.type);
    json_write_member_value(document, json, "name", value.name);
    switch (value.type) {
        case BindingType::UNIFORM:
        case BindingType::INSTANCE: {
            auto bufferData = std::static_pointer_cast<BufferBinding>(value.data);
            json_write_value(document, json, *bufferData);
            break;
        }
        case BindingType::IMAGE_SAMPLER: {
            auto textureData = std::static_pointer_cast<ImageSamplerBinding>(value.data);
            json_write_value(document, json, *textureData);
            break;
        }
        case BindingType::IMAGE: {
            auto imageData = std::static_pointer_cast<ImageBinding>(value.data);
            json_write_value(document, json, *imageData);
            break;
        }
        default:
            throw std::runtime_error("Unknown material binding type");
    }
}

void JsonPrimitiveValue<Binding>::read(const rapidjson::Value& json, Binding& value) {
    json_read_member_value(json, "name", value.name);
    json_read_member_value(json, "type", value.type);
    switch (value.type) {
        case BindingType::UNIFORM:
        case BindingType::INSTANCE: {
            auto bufferData = std::make_shared<BufferBinding>();
            json_read_value(json, *bufferData);
            value.data = std::move(bufferData);
            break;
        }
        case BindingType::IMAGE_SAMPLER: {
            auto textureData = std::make_shared<ImageSamplerBinding>();
            json_read_value(json, *textureData);
            value.data = std::move(textureData);
            break;
        }
        case BindingType::IMAGE: {
            auto imageData = std::make_shared<ImageBinding>();
            json_read_value(json, *imageData);
            value.data = std::move(imageData);
            break;
        }
        default:
            throw std::runtime_error("Unknown material binding type");
    }
}
