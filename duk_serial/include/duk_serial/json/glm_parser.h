//
// Created by rov on 10/5/2025.
//

#ifndef DUK_GLM_PARSER_H
#define DUK_GLM_PARSER_H

#include <duk_serial/json/json_parser.h>

namespace duk::serial {

template<typename T>
class VecParser : public Parser<T> {
public:
    void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const override;

    void read(const rapidjson::Value& json, T& self) const override;
};

template<typename T>
class QuatParser : public Parser<T> {
public:
    void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const override;

    void read(const rapidjson::Value& json, T& self) const override;

private:
    VecParser<glm::vec<3, typename T::value_type>> m_vecParser;
};

struct VecParserTag {};
struct QuaParserTag {};

template<glm::length_t L, typename T, glm::qualifier Q>
struct select_parser_tag<glm::vec<L, T, Q>> {
    using type = VecParserTag;
};

template<typename T, glm::qualifier Q>
struct select_parser_tag<glm::qua<T, Q>> {
    using type = QuaParserTag;
};

template<typename T>
std::unique_ptr<Parser<T>> make_parser(VecParserTag) {
    return std::make_unique<VecParser<T>>();
}

template<typename T>
std::unique_ptr<Parser<T>> make_parser(QuaParserTag) {
    return std::make_unique<QuatParser<T>>();
}

template<typename T>
void VecParser<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const {
    if constexpr (T::length() == 1) {
        json.SetString(fmt::format("{0}", value[0]), document.GetAllocator());
    } else if constexpr (T::length() == 2) {
        json.SetString(fmt::format("{0};{1}", value[0], value[1]), document.GetAllocator());
    } else if constexpr (T::length() == 3) {
        json.SetString(fmt::format("{0};{1};{2}", value[0], value[1], value[2]), document.GetAllocator());
    } else if constexpr (T::length() == 4) {
        json.SetString(fmt::format("{0};{1};{2};{4}", value[0], value[1], value[2], value[3]), document.GetAllocator());
    } else {
        static_assert(false, "Unsupported type");
    }
}

template<typename T>
void VecParser<T>::read(const rapidjson::Value& json, T& self) const {
    using ValueT = typename T::value_type;
    std::stringstream str(json.GetString());
    std::string segment;
    for (int i = 0; std::getline(str, segment, ';') && i < T::length(); i++) {
        if constexpr (std::is_same_v<ValueT, float>) {
            self[i] = std::stof(segment);
        } else if constexpr (std::is_same_v<ValueT, double>) {
            self[i] = std::stod(segment);
        } else if constexpr (std::is_same_v<ValueT, int32_t>) {
            self[i] = std::stoi(segment);
        } else if constexpr (std::is_same_v<ValueT, uint32_t>) {
            self[i] = std::stoul(segment);
        } else if constexpr (std::is_same_v<ValueT, int64_t>) {
            self[i] = std::stoll(segment);
        } else if constexpr (std::is_same_v<ValueT, uint64_t>) {
            self[i] = std::stoull(segment);
        } else if constexpr (std::is_same_v<ValueT, bool>) {
            self[i] = std::stoi(segment) != 0;
        } else {
            static_assert(false, "Unsupported type");
        }
    }
}

template<typename T>
void QuatParser<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const {
    m_vecParser.write(document, json, glm::degrees(glm::eulerAngles(value)));
}

template<typename T>
void QuatParser<T>::read(const rapidjson::Value& json, T& self) const {
    glm::vec<3, typename T::value_type> vec;
    m_vecParser.read(json, vec);
    self = glm::quat(glm::radians(vec));

}

}

#endif //DUK_GLM_PARSER_H