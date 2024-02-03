//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_RESOURCE_IMPORTER_H
#define DUK_IMPORT_RESOURCE_IMPORTER_H

#include <duk_resource/resource.h>

#include <set>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace duk::import {

enum class ResourceType {
    RES,
    IMG,
    MAT,
    SCN
};

struct ResourceDescription {
    ResourceType type;
    std::string path;
    duk::resource::Id id;
    std::set<duk::resource::Id> dependencies;
    std::set<std::string> aliases;
};

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs);

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs);

struct ResourceSet {
    std::unordered_map<duk::resource::Id, ResourceDescription> resources;
    std::unordered_map<std::string, duk::resource::Id> aliases;
};

class ResourceImporter {
public:

    virtual ~ResourceImporter();

    DUK_NO_DISCARD virtual bool accept(const std::filesystem::path& path) const = 0;

    DUK_NO_DISCARD virtual ResourceSet load(const std::filesystem::path& path) const = 0;

};

}

#endif //DUK_IMPORT_RESOURCE_IMPORTER_H
