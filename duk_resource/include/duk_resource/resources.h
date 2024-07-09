//
// Created by Ricardo on 13/04/2024.
//

#ifndef DUK_RESOURCE_RESOURCES_H
#define DUK_RESOURCE_RESOURCES_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>

#include <duk_tools/globals.h>

#include <filesystem>

namespace duk::resource {

enum class LoadMode {
    UNPACKED = 0,
    PACKED
};

struct ResourcesCreateInfo {
    std::filesystem::path path;
    duk::tools::Globals* globals;
    LoadMode loadMode;
};

class Resources {
public:
    explicit Resources(const ResourcesCreateInfo& resourcesCreateInfo);

    Handle<void> load(Id id);

    Handle<void> load(const std::string& alias);

    template<typename T>
    Handle<T> load(Id id);

    template<typename T>
    Handle<T> load(const std::string& alias);

    Id find_id(const std::string& alias) const;

    const Pools* pools() const;

    Pools* pools();

private:
    duk::tools::Globals* m_globals;
    Pools m_pools;
    LoadMode m_mode;
    std::unordered_map<Id, ResourceFile> m_resourceFiles;
    std::unordered_map<std::string, Id> m_resourceIdAliases;
};

template<typename T>
Handle<T> Resources::load(Id id) {
    return load(id).as<T>();
}

template<typename T>
Handle<T> Resources::load(const std::string& alias) {
    return load(alias).as<T>();
}

}// namespace duk::resource

#endif//DUK_RESOURCE_RESOURCES_H
