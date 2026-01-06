//
// Created by Ricardo on 03/05/2024.
//

#include <duk_renderer/material/material_data.h>

using namespace duk::serial;
using namespace duk::renderer;

void JsonPrimitiveValue<BufferBinding::Member>::write(rapidjson::Document& document, rapidjson::Value& json, const BufferBinding::Member& value) {
    json.SetObject();
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
    json_write_member_value(document, json, "type", value.type);
    json_write_member_value(document, json, "name", value.name);
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

void JsonPrimitiveValue<Binding>::write(rapidjson::Document& document, rapidjson::Value& json, const Binding& value) {
    json.SetObject();
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
    json_write_member_value(document, json, "type", value.type);
    json_write_member_value(document, json, "name", value.name);
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
