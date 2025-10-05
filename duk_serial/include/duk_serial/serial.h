//
// Created by Ricardo on 18/04/2024.
//

#ifndef DUK_SERIAL_JSON_IO_H
#define DUK_SERIAL_JSON_IO_H

#include <duk_serial/parser.h>
#include <duk_serial/container_parsers.h>
#include <duk_serial/glm_parsers.h>

#include <stdexcept>

namespace duk::serial {

template<typename T>
void to_json_document(rapidjson::Document& document, const T& self) {
    get_parser<T>()->write(document, document.SetObject(), self);
}

template<typename T>
void from_json_document(const rapidjson::Document& document, T& self) {
    get_parser<T>()->read(document, self);
}

template<typename T>
void read_json(T& self, const std::string_view& json) {
    rapidjson::Document document;
    document.Parse(json.data());
    if (document.HasParseError()) {
        throw std::runtime_error(fmt::format("failed to parse json: {}", rapidjson::GetParseError_En(document.GetParseError())));
    }
    from_json_document(document, self);
}

template<typename T>
T read_json(const std::string_view& json) {
    T self;
    read_json(self, json);
    return self;
}

inline void write_json(std::ostream& os, const rapidjson::Document& document, bool pretty = false) {
    rapidjson::StringBuffer buffer;
    if (pretty) {
        rapidjson::PrettyWriter write(buffer);
        document.Accept(write);
    }
    else {
        rapidjson::Writer writer(buffer);
        document.Accept(writer);
    }
    os << buffer.GetString();
}

inline std::string write_json(const rapidjson::Document& document, bool pretty = false) {
    std::ostringstream oss;
    write_json(oss, document, pretty);
    return oss.str();
}

template<typename T>
void write_json(std::ostream& os, const T& self, bool pretty = false) {
    rapidjson::Document document;
    to_json_document(document, self);
    write_json(os, document, pretty);
}

template<typename T>
std::string write_json(const T& self, bool pretty = false) {
    std::ostringstream oss;
    write_json(oss, self, pretty);
    return oss.str();
}

}// namespace duk::serial

#endif//DUK_SERIAL_JSON_IO_H
