//
// Created by Ricardo on 13/04/2024.
//
#include <duk_resource/handler.h>
#include <duk_resource/resources.h>

#include <duk_tools/file.h>

#include <duk_serial/json/serializer.h>

#include <duk_log/log.h>

namespace duk::resource {

namespace detail {

ResourceFile load_resource_file(const std::filesystem::path& path) {
    auto content = duk::tools::load_text(path);

    ResourceFile resourceFile = {};
    duk::serial::read_json(content, resourceFile);

    // convert relative path into absolute path
    resourceFile.file = (path.parent_path() / resourceFile.file).string();

    return resourceFile;
}

static std::vector<ResourceFile> scan_resource_files(const std::filesystem::path& path) {
    auto directory = path;
    if (!std::filesystem::is_directory(directory)) {
        directory = directory.parent_path();
    }
    std::vector<ResourceFile> resourceFiles;
    for (const auto& entry: std::filesystem::recursive_directory_iterator(directory)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".res") {
            auto resourceDescription = load_resource_file(entry.path());

            // check if resource actually exists, ignore if not
            if (!std::filesystem::exists(resourceDescription.file)) {
                duk::log::warn("ignored resource at \"{0}\", file at \"{1}\" does not exist!", entry.path().string(), resourceDescription.file);
                continue;
            }

            resourceFiles.emplace_back(std::move(resourceDescription));
        }
    }
    return resourceFiles;
}

static std::vector<ResourceFile> load_compressed_resource_files(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument(fmt::format("resource set file '{}' does not exist", path.string()));
    }

    auto compressedResources = path / "resources.bin";

    const auto json = duk::tools::load_compressed_text(compressedResources);

    auto resourceFiles = duk::serial::read_json<std::vector<ResourceFile>>(json);

    for (auto& resourceFile: resourceFiles) {
        // convert relative path into absolute path
        resourceFile.file = (path / resourceFile.file).string();
    }

    return resourceFiles;
}

static std::vector<ResourceFile> load_resource_files(const std::filesystem::path& path, LoadMode mode) {
    switch (mode) {
        case LoadMode::UNPACKED:
            return detail::scan_resource_files(path);
        case LoadMode::PACKED:
            return detail::load_compressed_resource_files(path);
    }
    return {};
}

}// namespace detail

Resources::Resources(const ResourcesCreateInfo& resourcesCreateInfo)
    : m_globals(resourcesCreateInfo.globals)
    , m_mode(resourcesCreateInfo.loadMode) {
    const auto resourceFiles = detail::load_resource_files(resourcesCreateInfo.path, m_mode);
    for (const auto& resourceFile: resourceFiles) {
        m_resourceFiles.emplace(resourceFile.id, resourceFile);
        for (auto& alias: resourceFile.aliases) {
            m_resourceIdAliases.emplace(alias, resourceFile.id);
        }
    }
}

Handle<void> Resources::load(const Id id) {
    Handle<void> handle;
    if (id < kMaxBuiltInResourceId) {
        // this is a builtin resource, skip
        return handle;
    }

    if (m_pools.contains(id)) {
        duk::log::verb("Resource '{}' already loaded, skipping", id.value());
        return handle;
    }

    auto resourceIt = m_resourceFiles.find(id);
    if (resourceIt == m_resourceFiles.end()) {
        throw std::invalid_argument(fmt::format("Resource id {} not found", id.value()));
    }

    const auto& resourceFile = resourceIt->second;

    auto handler = ResourceRegistry::instance()->find_handler(resourceFile.tag);
    if (!handler) {
        throw std::runtime_error(fmt::format("Failed to find a Handler for tag ({})", resourceFile.tag));
    }

    try {
        handle = handler->load(m_globals, m_pools, resourceFile.id, resourceFile.file, m_mode);
    } catch (const std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to load resource '{}' at '{}', reason: {}\n", resourceFile.id.value(), resourceFile.file, e.what()));
    }

    std::set<Id> dependencies;
    handler->solve_dependencies(handle, dependencies);

    for (const auto dependency: dependencies) {
        load(dependency);
    }

    handler->solve_references(handle, m_pools);

    return handle;
}

Handle<void> Resources::load(const std::string& alias) {
    auto id = find_id(alias);
    if (id == kInvalidId) {
        throw std::invalid_argument(fmt::format("resource alias '{}' not found", alias));
    }
    return load(id);
}

Id Resources::find_id(const std::string& alias) const {
    auto it = m_resourceIdAliases.find(alias);
    if (it != m_resourceIdAliases.end()) {
        return it->second;
    }
    return kInvalidId;
}

const Pools* Resources::pools() const {
    return &m_pools;
}

Pools* Resources::pools() {
    return &m_pools;
}

}// namespace duk::resource
