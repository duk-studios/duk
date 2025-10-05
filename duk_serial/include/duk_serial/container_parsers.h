//
// Created by rov on 10/5/2025.
//

#ifndef DUK_SERIAL_CONTAINER_PARSERS_H
#define DUK_SERIAL_CONTAINER_PARSERS_H

#include <duk_serial/parser.h>

namespace duk::serial {

template<typename T>
class ArrayParser : public Parser<T> {
public:
    void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const override;

    void read(const rapidjson::Value& json, T& self) const override;
};

struct ArrayParserTag {};

template<typename T>
struct select_parser_tag<std::vector<T>> {
    using type = ArrayParserTag;
};

template<typename T>
std::unique_ptr<Parser<T>> make_parser(ArrayParserTag) {
    return std::make_unique<ArrayParser<T>>();
}

template<typename T>
void ArrayParser<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const {
    const auto elementParser = get_parser<typename T::value_type>();
    auto& jsonArray = json.SetArray();
    for (const auto& element : value) {
        rapidjson::Value jsonElement;
        elementParser->write(document, jsonElement, element);
        jsonArray.PushBack(std::move(jsonElement), document.GetAllocator());
    }
}

template<typename T>
void ArrayParser<T>::read(const rapidjson::Value& json, T& self) const {
    const auto elementParser = get_parser<typename T::value_type>();
    const auto jsonArray = json.GetArray();
    for (auto& jsonElement : jsonArray) {
        elementParser->read(jsonElement, self.emplace_back());
    }
}


}

#endif //DUK_SERIAL_CONTAINER_PARSERS_H