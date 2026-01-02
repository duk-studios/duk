//
// Created by rov on 29/12/2025.
//
#include <duk_serial/json.h>

namespace duk::serial {
void JsonPrimitiveValue<bool>::write(rapidjson::Document& document, rapidjson::Value& json, const bool& value) {
    json.SetBool(value);
}

void JsonPrimitiveValue<bool>::read(const rapidjson::Value& json, bool& value) {
    value = json.GetBool();
}

void JsonPrimitiveValue<int>::write(rapidjson::Document& document, rapidjson::Value& json, const int32_t& value) {
    json.SetInt(value);
}

void JsonPrimitiveValue<int>::read(const rapidjson::Value& json, int32_t& value) {
    value = json.GetInt();
}

void JsonPrimitiveValue<unsigned>::write(rapidjson::Document& document, rapidjson::Value& json, const uint32_t& value) {
    json.SetUint(value);
}

void JsonPrimitiveValue<unsigned>::read(const rapidjson::Value& json, uint32_t& value) {
    value = json.GetUint();
}

void JsonPrimitiveValue<long long>::write(rapidjson::Document& document, rapidjson::Value& json, const int64_t& value) {
    json.SetInt64(value);
}

void JsonPrimitiveValue<long long>::read(const rapidjson::Value& json, int64_t& value) {
    value = json.GetInt64();
}

void JsonPrimitiveValue<unsigned long long>::write(rapidjson::Document& document, rapidjson::Value& json, const uint64_t& value) {
    json.SetUint64(value);
}

void JsonPrimitiveValue<unsigned long long>::read(const rapidjson::Value& json, uint64_t& value) {
    value = json.GetUint64();
}

void JsonPrimitiveValue<float>::write(rapidjson::Document& document, rapidjson::Value& json, const float& value) {
    json.SetFloat(value);
}

void JsonPrimitiveValue<float>::read(const rapidjson::Value& json, float& value) {
    value = json.GetFloat();
}

void JsonPrimitiveValue<double>::write(rapidjson::Document& document, rapidjson::Value& json, const double& value) {
    json.SetDouble(value);
}

void JsonPrimitiveValue<double>::read(const rapidjson::Value& json, double& value) {
    value = json.GetDouble();
}

void JsonPrimitiveValue<std::string>::write(rapidjson::Document& document, rapidjson::Value& json, const std::string& value) {
    json.SetString(value, document.GetAllocator());
}

void JsonPrimitiveValue<std::string>::read(const rapidjson::Value& json, std::string& value) {
    value = json.GetString();
}

}