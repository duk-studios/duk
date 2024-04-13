//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_SET_H
#define DUK_RESOURCE_SET_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>

#include <filesystem>

namespace duk::resource {

struct ResourceSetCreateInfo {
    std::filesystem::path path;
    Pools* pools;
};

class ResourceSet {
public:

    explicit ResourceSet(const ResourceSetCreateInfo& resourceSetCreateInfo);

    void load(Id id);

    Pools* pools() const;

private:
    Pools* m_pools;
    std::unordered_map<Id, ResourceFile> m_resourceFiles;
    std::unordered_map<std::string, Id> m_resourceIdAliases;
};

}

#endif //DUK_RESOURCE_SET_H
