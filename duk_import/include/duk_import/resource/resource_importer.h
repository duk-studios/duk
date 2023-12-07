//
// Created by rov on 12/5/2023.
//

#ifndef DUK_IMPORT_RESOURCE_IMPORTER_H
#define DUK_IMPORT_RESOURCE_IMPORTER_H

#include <duk_pool/resource.h>

#include <set>
#include <string>
#include <filesystem>

namespace duk::import {

enum class ResourceType {
    RES,
    IMG,
    MAT
};

struct ResourceDescription {
    ResourceType type;
    std::string path;
    duk::pool::ResourceId id;
};

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs);

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs);

struct ResourceSet {
    std::set<ResourceDescription> resourceSets;
    std::set<ResourceDescription> images;
    std::set<ResourceDescription> materials;
};

class ResourceImporter {
public:

    virtual ~ResourceImporter();

    DUK_NO_DISCARD virtual bool accept(const std::filesystem::path& path) const = 0;

    DUK_NO_DISCARD virtual ResourceSet load(const std::filesystem::path& path) const = 0;

};

}

#endif //DUK_IMPORT_RESOURCE_IMPORTER_H
