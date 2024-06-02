//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_FILE_H
#define DUK_RESOURCE_FILE_H

#include <duk_resource/resource.h>

#include <duk_serial/json/types.h>

#include <set>
#include <string>

namespace duk::resource {

struct ResourceFile {
    std::string tag;
    std::string file;
    Id id;
    std::set<std::string> aliases;
};

bool operator==(const ResourceFile& lhs, const ResourceFile& rhs);

bool operator<(const ResourceFile& lhs, const ResourceFile& rhs);

}// namespace duk::resource

namespace duk::serial {

template<>
inline void from_json<duk::resource::ResourceFile>(const rapidjson::Value& json, duk::resource::ResourceFile& resourceFile) {
    from_json_member(json, "tag", resourceFile.tag);
    from_json_member(json, "id", resourceFile.id);
    from_json_member(json, "file", resourceFile.file);
    from_json_member(json, "aliases", resourceFile.aliases, true);
}

template<>
inline void to_json<duk::resource::ResourceFile>(rapidjson::Document& document, rapidjson::Value& json, const duk::resource::ResourceFile& resourceFile) {
    to_json_member(document, json, "tag", resourceFile.tag);
    to_json_member(document, json, "id", resourceFile.id);
    to_json_member(document, json, "file", resourceFile.file);
    to_json_member(document, json, "aliases", resourceFile.aliases);
}

}// namespace duk::serial

#endif//DUK_RESOURCE_FILE_H
