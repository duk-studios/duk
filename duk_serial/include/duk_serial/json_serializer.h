/// 24/02/2024
/// json_serializer.h

#ifndef DUK_SERIAL_JSON_SERIALIZER_H
#define DUK_SERIAL_JSON_SERIALIZER_H

#include <duk_macros/assert.h>
#include <duk_serial/serializer.h>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <set>
#include <sstream>
#include <stack>
#define RAPIDJSON_ASSERT(x) DUK_ASSERT(x)
#include <rapidjson/document.h>

namespace duk::serial {

template<typename T>
void from_json(const rapidjson::Value& json, T& value) {
    value = json.Get<T>();
}

template<typename T>
void to_json(const T& value, rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) {
    json.Set(value, allocator);
}

template<typename JsonVisitor, typename T>
void visit_object(JsonVisitor* visitor, T& object) = delete;

class JsonReader;
template<typename T>
void read_array(JsonReader* reader, T& array, size_t size) = delete;

class JsonWriter;
template<typename T>
void write_array(JsonWriter* writer, T& array) = delete;

class JsonReader {
public:
    JsonReader(const char* jsonStr);
    JsonReader(rapidjson::Document&& document);

    /* serializer interface */
    template<typename T>
    void visit_member(T& value, const MemberDescription& member) {
        rapidjson::Value jsonValue;
        if (!find_member(jsonValue, member)) {
            return;
        }
        from_json(jsonValue, value);
    }

    template<typename T>
    void visit_member_array(T& array, const MemberDescription& member) {
        rapidjson::Value jsonArray;
        if (!find_member(jsonArray, member)) {
            return;
        }
        DUK_ASSERT(jsonArray.IsArray());
        m_valueStack.push(&jsonArray);
        read_array(this, array, jsonArray.GetArray().Size());
        m_valueStack.pop();
    }

    template<typename T>
    void visit_member_object(T& value, const MemberDescription& member) {
        rapidjson::Value jsonObject;
        if (!find_member(jsonObject, member)) {
            return;
        }
        DUK_ASSERT(jsonObject.IsObject());
        m_valueStack.push(&jsonObject);
        visit_object(this, value);
        m_valueStack.pop();
    }

    template<typename T>
    void visit(T& value) {
        m_valueStack.push(&m_document);
        visit_object(this, value);
        m_valueStack.pop();
    }

private:
    bool find_member(rapidjson::Value& value, const MemberDescription& member) {
        auto& root = m_valueStack.top();
        if (member.name()) {
            auto memberIt = root->FindMember(member.name());
            if (memberIt == root->MemberEnd()) {
                return false;
            }
            value = memberIt->value;
            return true;
        }
        auto array = root->GetArray();
        value = array[member.index()];
        return true;
    }

private:
    rapidjson::Document m_document;
    std::stack<rapidjson::Value*> m_valueStack;
};

class JsonWriter {
public:
    DUK_NO_DISCARD std::string print() const;

    DUK_NO_DISCARD std::string pretty_print() const;

    /* serializer interface */
    template<typename T>
    void visit_member(T& value, const MemberDescription& member) {
        rapidjson::Value jsonValue;
        to_json(value, jsonValue, m_document.GetAllocator());
        insert_value(*m_valueStack.top(), member.name(), std::move(jsonValue));
    }

    template<typename T>
    void visit_member_array(T& array, const MemberDescription& member) {
        rapidjson::Value jsonArray;
        m_valueStack.push(&jsonArray.SetArray());
        write_array(this, array);
        m_valueStack.pop();
        insert_value(*m_valueStack.top(), member.name(), std::move(jsonArray));
    }

    template<typename T>
    void visit_member_object(T& value, const MemberDescription& member) {
        rapidjson::Value jsonObject;
        m_valueStack.push(&jsonObject.SetObject());
        visit_object(this, value);
        m_valueStack.pop();
        insert_value(*m_valueStack.top(), member.name(), std::move(jsonObject));
    }

    template<typename T>
    void visit(T& value) {
        m_valueStack.push(&m_document.SetObject());
        visit_object(this, value);
        m_valueStack.pop();
    }

private:
    void insert_value(rapidjson::Value& root, const char* name, rapidjson::Value value);

private:
    rapidjson::Document m_document;
    std::stack<rapidjson::Value*> m_valueStack;
};

template<>
inline void from_json<std::string>(const rapidjson::Value& json, std::string& value) {
    value = json.GetString();
}

template<>
inline void to_json<std::string>(const std::string& value, rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) {
    json.SetString(value.c_str(), allocator);
}

namespace detail {

template<typename VecT>
static VecT parse_vec(const rapidjson::Value& member) {
    std::stringstream str(member.GetString());
    std::string segment;
    VecT result = {};
    for (int i = 0; std::getline(str, segment, ';') && i < VecT::length(); i++) {
        result[i] = std::stof(segment);
    }
    return result;
}

}// namespace detail

template<>
inline void from_json<glm::vec2>(const rapidjson::Value& jsonObject, glm::vec2& vec) {
    vec = detail::parse_vec<glm::vec2>(jsonObject);
}

template<>
inline void to_json<glm::vec2>(const glm::vec2& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1}", vec.x, vec.y);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::ivec2>(const rapidjson::Value& jsonObject, glm::ivec2& vec) {
    vec = detail::parse_vec<glm::ivec2>(jsonObject);
}

template<>
inline void to_json<glm::ivec2>(const glm::ivec2& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1}", vec.x, vec.y);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::vec3>(const rapidjson::Value& jsonObject, glm::vec3& vec) {
    vec = detail::parse_vec<glm::vec3>(jsonObject);
}

template<>
inline void to_json<glm::vec3>(const glm::vec3& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1};{2}", vec.x, vec.y, vec.z);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::ivec3>(const rapidjson::Value& jsonObject, glm::ivec3& vec) {
    vec = detail::parse_vec<glm::ivec3>(jsonObject);
}

template<>
inline void to_json<glm::ivec3>(const glm::ivec3& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1};{2}", vec.x, vec.y, vec.z);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::vec4>(const rapidjson::Value& jsonObject, glm::vec4& vec) {
    vec = detail::parse_vec<glm::vec4>(jsonObject);
}

template<>
inline void to_json<glm::vec4>(const glm::vec4& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1};{2};{3}", vec.x, vec.y, vec.z, vec.w);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::ivec4>(const rapidjson::Value& jsonObject, glm::ivec4& vec) {
    vec = detail::parse_vec<glm::ivec4>(jsonObject);
}

template<>
inline void to_json<glm::ivec4>(const glm::ivec4& vec, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto vecStr = fmt::format("{0};{1};{2};{3}", vec.x, vec.y, vec.z, vec.w);
    jsonObject.SetString(vecStr.c_str(), vecStr.size(), allocator);
}

template<>
inline void from_json<glm::quat>(const rapidjson::Value& jsonObject, glm::quat& quat) {
    quat = glm::quat(glm::radians(detail::parse_vec<glm::vec3>(jsonObject)));
}

template<>
inline void to_json<glm::quat>(const glm::quat& quat, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    to_json<glm::vec3>(glm::eulerAngles(quat), jsonObject, allocator);
}

template<typename T>
void from_json(const rapidjson::Value& jsonObject, std::vector<T>& vector) {
    DUK_ASSERT(jsonObject.IsArray());
    auto jsonArray = jsonObject.GetArray();
    for (auto& jsonElement: jsonArray) {
        from_json(jsonElement, vector.emplace_back());
    }
}

template<>
inline void read_array(JsonReader* reader, std::set<std::string>& set, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::string value;
        reader->visit_member(value, MemberDescription(i));
        set.insert(std::move(value));
    }
}

template<>
inline void write_array(JsonWriter* writer, std::set<std::string>& set) {
    for (auto& value: set) {
        writer->visit_member(value, MemberDescription(nullptr));
    }
}

}// namespace duk::serial

#endif// DUK_SERIAL_JSON_SERIALIZER_H
