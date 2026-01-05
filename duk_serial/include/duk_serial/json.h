//
// Created by rov on 24/12/2025.
//

#ifndef DUK_SERIAL_JSON_H
#define DUK_SERIAL_JSON_H

#include <duk_serial/rapidjson_import.h>

#include <duk_type/describe.h>
#include <duk_type/describe_class.h>
#include <duk_type/describe_container.h>
#include <duk_type/describe_enum.h>

#include <variant>
#include <unordered_map>

namespace duk::serial {

template<typename T, bool Pretty = false>
std::string json_write(const T& value);

template<typename T>
T json_read(const std::string_view& json);

template<typename T, bool Pretty = false>
void json_write(std::string& json, const T& value);

template<typename T>
void json_read(const std::string_view& json, T& value);

template<typename T>
void json_write_value(rapidjson::Document& document, rapidjson::Value& json, const T& value);

template<typename T>
void json_read_value(const rapidjson::Value& json, T& value);

template<typename T>
void json_write_member_value(rapidjson::Document& document, rapidjson::Value& json, std::string_view name, const T& value);

template<typename T>
void json_read_member_value(const rapidjson::Value& json, std::string_view name, T& value);

// generic primitives have no implementation
// specializations will provide the actual serialization
template<typename T>
struct JsonPrimitiveValue {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    }

    static void read(const rapidjson::Value& json, T& value) {
    }
};

// Used for classes/structs that specializes duk::type::Type<T> and inherit from duk::type::Class<T, ...>
template<typename T>
struct JsonObjectValue {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const T& value);

    static void read(const rapidjson::Value& json, T& value);
};

// Used for containers that specializes duk::type::Type<T> and inherit from duk::type::Container<T>
template<typename T>
struct JsonContainerValue {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const T& value);

    static void read(const rapidjson::Value& json, T& value);
};

// Used for enums that specializes duk::type::Type<T> and inherit from duk::type::Enum<T, ...>
template<typename T>
struct JsonEnumValue {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const T& value);

    static void read(const rapidjson::Value& json, T& value);
};

// specializations
template<>
struct JsonPrimitiveValue<bool> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const bool& value);

    static void read(const rapidjson::Value& json, bool& value);
};

template<>
struct JsonPrimitiveValue<int32_t> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const int32_t& value);

    static void read(const rapidjson::Value& json, int32_t& value);
};

template<>
struct JsonPrimitiveValue<uint32_t> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const uint32_t& value);

    static void read(const rapidjson::Value& json, uint32_t& value);
};

template<>
struct JsonPrimitiveValue<int64_t> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const int64_t& value);

    static void read(const rapidjson::Value& json, int64_t& value);
};

template<>
struct JsonPrimitiveValue<uint64_t> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const uint64_t& value);

    static void read(const rapidjson::Value& json, uint64_t& value);
};

template<>
struct JsonPrimitiveValue<float> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const float& value);

    static void read(const rapidjson::Value& json, float& value);
};

template<>
struct JsonPrimitiveValue<double> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const double& value);

    static void read(const rapidjson::Value& json, double& value);
};

template<>
struct JsonPrimitiveValue<std::string> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::string& value);

    static void read(const rapidjson::Value& json, std::string& value);
};

template<typename T>
struct JsonPrimitiveValue<std::shared_ptr<T>> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::shared_ptr<T>& value);

    static void read(const rapidjson::Value& json, std::shared_ptr<T>& value);
};

template<typename T>
struct JsonPrimitiveValue<std::unique_ptr<T>> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<T>& value);

    static void read(const rapidjson::Value& json, std::unique_ptr<T>& value);
};

template<typename K, typename V>
struct JsonPrimitiveValue<std::unordered_map<K, V>> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::unordered_map<K, V>& value);

    static void read(const rapidjson::Value& json, std::unordered_map<K, V>& value);
};

template<typename... Ts>
struct JsonPrimitiveValue<std::variant<Ts...>> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::variant<Ts...>& value);

    static void read(const rapidjson::Value& json, std::variant<Ts...>& value);
};

template<typename T, bool Pretty>
std::string json_write(const T& value) {
    std::string json;
    json_write<T, Pretty>(json, value);
    return json;
}

template<typename T>
T json_read(const std::string_view& json) {
    T value;
    json_read<T>(json, value);
    return value;
}

template<typename T, bool Pretty>
void json_write(std::string& json, const T& value) {
    rapidjson::Document document;
    json_write_value(document, document, value);
    rapidjson::StringBuffer buffer;
    if constexpr (Pretty) {
        rapidjson::PrettyWriter write(buffer);
        document.Accept(write);
    } else {
        rapidjson::Writer writer(buffer);
        document.Accept(writer);
    }
    json = buffer.GetString();
}

template<typename T>
void json_read(const std::string_view& json, T& value) {
    rapidjson::Document document;
    document.Parse(json.data());
    if (document.HasParseError()) {
        throw std::runtime_error(fmt::format("failed to parse json: {}", rapidjson::GetParseError_En(document.GetParseError())));
    }
    json_read_value(document, value);
}

template<typename T>
void json_write_value(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    if constexpr (duk::type::is_class<T>()) {
        JsonObjectValue<T>::write(document, json, value);
    } else if constexpr (duk::type::is_container<T>()) {
        JsonContainerValue<T>::write(document, json, value);
    } else if constexpr (duk::type::is_enum<T>()) {
        JsonEnumValue<T>::write(document, json, value);
    } else {
        JsonPrimitiveValue<T>::write(document, json, value);
    }
}

template<typename T>
void json_read_value(const rapidjson::Value& json, T& value) {
    if constexpr (duk::type::is_class<T>()) {
        JsonObjectValue<T>::read(json, value);
    } else if constexpr (duk::type::is_container<T>()) {
        JsonContainerValue<T>::read(json, value);
    } else if constexpr (duk::type::is_enum<T>()) {
        JsonEnumValue<T>::read(json, value);
    } else {
        JsonPrimitiveValue<T>::read(json, value);
    }
}

template<typename T>
void json_write_member_value(rapidjson::Document& document, rapidjson::Value& json, std::string_view name, const T& value) {
    DUK_ASSERT(json.IsObject());
    rapidjson::Value jsonMemberValue;
    json_write_value(document, jsonMemberValue, value);
    rapidjson::Value jsonMemberName;
    jsonMemberName.SetString(name.data(), name.size(), document.GetAllocator());
    json.AddMember(std::move(jsonMemberName), std::move(jsonMemberValue), document.GetAllocator());
}

template<typename T>
void json_read_member_value(const rapidjson::Value& json, std::string_view name, T& value) {
    DUK_ASSERT(json.IsObject());
    const auto jsonMemberIt = json.FindMember(name.data());
    if (jsonMemberIt == json.MemberEnd()) {
        return;
    }
    json_read_value(jsonMemberIt->value, value);
}

template<typename T>
void JsonObjectValue<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    constexpr auto description = duk::type::describe<T>();
    json.SetObject();
    description.visit_members(
            [&](auto member) {
                constexpr auto memberDescription = member.describe();
                rapidjson::Value jsonMemberName;
                jsonMemberName.SetString(member.name().data(), static_cast<rapidjson::SizeType>(member.name().size()), document.GetAllocator());
                rapidjson::Value jsonMemberValue;
                json_write_value(document, jsonMemberValue, member.value());
                json.AddMember(std::move(jsonMemberName), std::move(jsonMemberValue), document.GetAllocator());
            },
            value);
}

template<typename T>
void JsonObjectValue<T>::read(const rapidjson::Value& json, T& value) {
    constexpr auto description = duk::type::describe<T>();
    description.visit_members(
            [&](auto member) {
                constexpr auto memberDescription = member.describe();
                auto jsonMemberIt = json.FindMember(member.name().data());
                if (jsonMemberIt == json.MemberEnd()) {
                    return;
                }
                json_read_value(jsonMemberIt->value, member.value());
            },
            value);
}

template<typename T>
void JsonContainerValue<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    constexpr auto description = duk::type::describe<T>();
    json.SetArray();
    description.visit_elements(
            [&](const auto& element) {
                rapidjson::Value jsonElement;
                json_write_value(document, jsonElement, element);
                json.PushBack(std::move(jsonElement), document.GetAllocator());
            },
            value);
}

template<typename T>
void JsonContainerValue<T>::read(const rapidjson::Value& json, T& value) {
    constexpr auto description = duk::type::describe<T>();
    for (auto& jsonElement: json.GetArray()) {
        using ElementT = decltype(description)::value_type;
        ElementT element;
        json_read_value(jsonElement, element);
        description.insert_back(value, std::move(element));
    }
}

template<typename T>
void JsonEnumValue<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) {
    constexpr auto description = duk::type::describe<T>();
    const auto name = description.name_of(value);
    json.SetString(name.data(), name.size(), document.GetAllocator());
}

template<typename T>
void JsonEnumValue<T>::read(const rapidjson::Value& json, T& value) {
    constexpr auto description = duk::type::describe<T>();
    const auto name = json.GetString();
    value = description.value_of(name);
}

template<typename T>
void JsonPrimitiveValue<std::shared_ptr<T>>::write(rapidjson::Document& document, rapidjson::Value& json, const std::shared_ptr<T>& value) {
    json_write_value(document, json, *value);
}

template<typename T>
void JsonPrimitiveValue<std::shared_ptr<T>>::read(const rapidjson::Value& json, std::shared_ptr<T>& value) {
    value = std::make_shared<T>();
    json_read_value(json, *value);
}

template<typename T>
void JsonPrimitiveValue<std::unique_ptr<T>>::write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<T>& value) {
    json_write_value(document, json, *value);
}

template<typename T>
void JsonPrimitiveValue<std::unique_ptr<T>>::read(const rapidjson::Value& json, std::unique_ptr<T>& value) {
    value = std::make_unique<T>();
    json_read_value(json, *value);
}

template<typename K, typename V>
void JsonPrimitiveValue<std::unordered_map<K, V>>::write(rapidjson::Document& document, rapidjson::Value& json, const std::unordered_map<K, V>& map) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& [key, value]: map) {
        rapidjson::Value elementJson;
        elementJson.SetObject();
        json_write_member_value(document, elementJson, "key", key);
        json_write_member_value(document, elementJson, "value", value);
        jsonArray.PushBack(std::move(elementJson), document.GetAllocator());
    }
}

template<typename K, typename V>
void JsonPrimitiveValue<std::unordered_map<K, V>>::read(const rapidjson::Value& json, std::unordered_map<K, V>& map) {
    DUK_ASSERT(json.IsArray());
    const auto jsonArray = json.GetArray();
    map.clear();
    for (const auto& jsonElement: jsonArray) {
        K key;
        json_read_member_value(jsonElement, "key", key);

        V value;
        json_read_member_value(jsonElement, "value", value);

        map.emplace(std::move(key), std::move(value));
    }
}

template<typename... Ts>
void JsonPrimitiveValue<std::variant<Ts...>>::write(rapidjson::Document& document, rapidjson::Value& json, const std::variant<Ts...>& value) {
    json.SetObject();
    std::visit(
            [&document, &json](const auto& typedValue) {
                using T = std::decay_t<decltype(typedValue)>;
                constexpr auto description = duk::type::describe<T>();
                json_write_member_value(document, json, "type", description.name());
                json_write_member_value(document, json, "value", typedValue);
            },
            value);
}

template<typename... Ts>
void JsonPrimitiveValue<std::variant<Ts...>>::read(const rapidjson::Value& json, std::variant<Ts...>& value) {
    DUK_ASSERT(json.IsObject());
    std::string typeName;
    json_read_member_value(json, "type", typeName);
    bool matched = false;
    auto tryMatch = [&]<typename T>() {
        if (typeName == duk::type::describe<T>().name() && !matched) {
            T typedValue;
            json_read_member_value(json, "value", typedValue);
            value = std::move(typedValue);
            matched = true;
        }
    };
    (tryMatch.template operator()<Ts>(), ...);
    if (!matched) {
        throw std::runtime_error(fmt::format("variant type '{}' not matched", typeName));
    }
}

}// namespace duk::serial

#endif//DUK_SERIAL_JSON_H