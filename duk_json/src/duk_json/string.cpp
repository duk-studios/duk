/// 03/02/2024
/// string.cpp

#include <duk_json/string.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace duk::json {

std::string to_string(const rapidjson::Value& value) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);
    return buffer.GetString();
}

}