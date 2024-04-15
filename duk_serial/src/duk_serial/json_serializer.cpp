/// 24/02/2024
/// json_serializer.cpp

#include <duk_serial/json_serializer.h>
#include <fmt/format.h>
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <stdexcept>

namespace duk::serial {

JsonReader::JsonReader(const char* jsonStr) {
    m_document.Parse(jsonStr);

    if (m_document.HasParseError()) {
        throw std::runtime_error(fmt::format("failed to parse json: {}", rapidjson::GetParseError_En(m_document.GetParseError())));
    }
}

JsonReader::JsonReader(rapidjson::Document&& document)
    : m_document(std::move(document)) {
    m_valueStack.push(&m_document);
}

std::string JsonWriter::print() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_document.Accept(writer);
    return buffer.GetString();
}

std::string JsonWriter::pretty_print() const {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    m_document.Accept(writer);
    return buffer.GetString();
}

void JsonWriter::insert_value(rapidjson::Value& root, const char* name, rapidjson::Value value) {
    if (name) {
        root.AddMember(rapidjson::StringRef(name), value, m_document.GetAllocator());
    } else {
        // when name is null, it means that root is an array
        root.PushBack(value, m_document.GetAllocator());
    }
}

}// namespace duk::serial