//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_FILE_H
#define DUK_RESOURCE_FILE_H

#include <duk_resource/handle.h>

#include <duk_serial/json.h>

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

namespace duk::type {

// clang-format off
template<>
struct Type<duk::resource::ResourceFile> : Class<duk::resource::ResourceFile,
    Member<"tag", &duk::resource::ResourceFile::tag>,
    Member<"id", &duk::resource::ResourceFile::id>,
    Member<"file", &duk::resource::ResourceFile::file>,
    Member<"aliases", &duk::resource::ResourceFile::aliases>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_RESOURCE_FILE_H
