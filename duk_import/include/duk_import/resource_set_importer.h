//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_RESOURCE_SET_IMPORTER_H
#define DUK_IMPORT_RESOURCE_SET_IMPORTER_H

#include "duk_resource/resource.h"

#include <set>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace duk::import {

struct ResourceDescription {
    std::string type;
    std::string path;
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

}

#endif //DUK_IMPORT_RESOURCE_SET_IMPORTER_H
