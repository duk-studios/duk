/// 03/02/2024
/// string.h

#ifndef DUK_JSON_STRING_H
#define DUK_JSON_STRING_H

#include <rapidjson/document.h>
#include <string>

namespace duk::json {

std::string to_string(const rapidjson::Value& value);

}

#endif // DUK_JSON_STRING_H

