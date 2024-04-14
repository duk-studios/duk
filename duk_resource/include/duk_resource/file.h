//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_FILE_H
#define DUK_RESOURCE_FILE_H

#include <duk_resource/resource.h>

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

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::resource::ResourceFile& resourceFile) {
    visitor->visit_member(resourceFile.tag, "tag");
    visitor->visit_member(resourceFile.id, "id");
    visitor->visit_member(resourceFile.file, "file");
    visitor->visit_member_array(resourceFile.aliases, "aliases");
}

}// namespace duk::serial

#endif//DUK_RESOURCE_FILE_H
