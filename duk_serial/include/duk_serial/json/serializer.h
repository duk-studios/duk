//
// Created by Ricardo on 18/04/2024.
//

#ifndef DUK_SERIAL_JSON_SERIALIZER_H
#define DUK_SERIAL_JSON_SERIALIZER_H

#include <duk_serial/json/types.h>

#include <duk_log/log.h>

#include <duk_tools/types.h>

namespace duk::serial {

template<typename T>
void from_json_member(const rapidjson::Value& json, const char* name, T& value, bool optional) {
    auto member = json.FindMember(name);
    if (member == json.MemberEnd()) {
        if (optional) {
            duk::log::info("Missing optional member '{}' in json object, skipping", name);
            return;
        }
        throw std::runtime_error(fmt::format("Mandatory json member '{}' missing", name));
    }
    duk::serial::from_json(member->value, value);
}

template<typename T>
void to_json_member(rapidjson::Document& document, rapidjson::Value& json, const char* name, const T& value) {
    if (!json.IsObject()) {
        json.SetObject();
    }
    rapidjson::Value member;
    to_json(document, member, value);
    json.AddMember(rapidjson::StringRef(name), std::move(member), document.GetAllocator());
}

template<typename T>
void read_json(const std::string_view& json, T& value) {
    rapidjson::Document document;
    document.Parse(json.data());

    if (document.HasParseError()) {
        throw std::runtime_error(fmt::format("failed to parse json: {}", rapidjson::GetParseError_En(document.GetParseError())));
    }

    from_json(document, value);
}

template<typename T>
T read_json(const std::string_view& json) {
    T value;
    read_json(json, value);
    return value;
}

template<typename T>
void write_json(std::ostringstream& buffer, const T& value, bool pretty = false) {
    rapidjson::Document document;
    to_json(document, document, value);
    if (pretty) {
        rapidjson::StringBuffer jsonBuffer;
        rapidjson::PrettyWriter writer(jsonBuffer);
        document.Accept(writer);
        buffer << jsonBuffer.GetString();
    } else {
        rapidjson::StringBuffer jsonBuffer;
        rapidjson::Writer writer(jsonBuffer);
        document.Accept(writer);
        buffer << jsonBuffer.GetString();
    }
}

}// namespace duk::serial

#endif//DUK_SERIAL_JSON_SERIALIZER_H
