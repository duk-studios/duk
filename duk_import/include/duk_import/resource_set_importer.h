//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_RESOURCE_SET_IMPORTER_H
#define DUK_IMPORT_RESOURCE_SET_IMPORTER_H

#include "duk_resource/resource.h"

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>

namespace duk::import {

struct ResourceDescription {
    std::string tag;
    std::string file;
    duk::resource::Id id;
    std::set<std::string> aliases;
};

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs);

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs);

struct ResourceSet {
    std::unordered_map<duk::resource::Id, ResourceDescription> resources;
    std::unordered_map<std::string, duk::resource::Id> aliases;
};

class ResourceSetImporter {
public:
    virtual ~ResourceSetImporter();

    DUK_NO_DISCARD bool accept(const std::filesystem::path& path) const;

    DUK_NO_DISCARD ResourceSet load(const std::filesystem::path& path) const;
};

}// namespace duk::import

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::import::ResourceDescription& resourceDescription) {
    visitor->visit_member(resourceDescription.tag, "tag");
    visitor->visit_member(resourceDescription.id, "id");
    visitor->visit_member(resourceDescription.file, "file");
    visitor->visit_member_array(resourceDescription.aliases, "aliases");
}

}// namespace duk::serial

#endif//DUK_IMPORT_RESOURCE_SET_IMPORTER_H
