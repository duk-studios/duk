//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_SET_H
#define DUK_RESOURCE_SET_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>

#include <filesystem>

namespace duk::resource {

enum class LoadMode {
    UNPACKED = 0,
    PACKED
};

struct ResourceSetCreateInfo {
    std::filesystem::path path;
    Pools* pools;
    LoadMode loadMode;
};

class ResourceSet {
public:
    explicit ResourceSet(const ResourceSetCreateInfo& resourceSetCreateInfo);

    Handle<void> load(Id id);

    Handle<void> load(const std::string& alias);

    Pools* pools() const;

    Id find_id(const std::string& alias) const;

private:
    Pools* m_pools;
    LoadMode m_mode;
    std::unordered_map<Id, ResourceFile> m_resourceFiles;
    std::unordered_map<std::string, Id> m_resourceIdAliases;
};

}// namespace duk::resource

#endif//DUK_RESOURCE_SET_H
