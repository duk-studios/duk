//
// Created by Ricardo on 13/04/2024.
//
#include <duk_resource/handler.h>
#include <duk_resource/set.h>

#include <duk_tools/file.h>

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

}// namespace detail

ResourceSet::ResourceSet(const ResourceSetCreateInfo& resourceSetCreateInfo)
    : m_pools(resourceSetCreateInfo.pools) {
    auto directory = resourceSetCreateInfo.path;
    if (!std::filesystem::is_directory(directory)) {
        directory = directory.parent_path();
    }

    // recursively search for .res files and add them to the resourceSet
    for (const auto& entry: std::filesystem::recursive_directory_iterator(directory)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".res") {
            auto resourceDescription = detail::load_resource_file(entry.path());

            // check if resource actually exists, ignore if not
            if (!std::filesystem::exists(resourceDescription.file)) {
                duk::log::warn("ignored resource at \"{0}\", file at \"{1}\" does not exist!", entry.path().string(), resourceDescription.file);
                continue;
            }

            m_resourceFiles.emplace(resourceDescription.id, resourceDescription);
            for (auto& alias: resourceDescription.aliases) {
                m_resourceIdAliases.emplace(alias, resourceDescription.id);
            }
        }
    }
}

void ResourceSet::load(const Id id) {
    if (id < kMaxBuiltInResourceId) {
        // this is a builtin resource, skip
        return;
    }

    auto resourceIt = m_resourceFiles.find(id);
    if (resourceIt == m_resourceFiles.end()) {
        throw std::invalid_argument("resource id not found");
    }

    const auto& resourceFile = resourceIt->second;

    auto handler = ResourceRegistry::instance()->find_handler(resourceFile.tag);
    if (!handler) {
        throw std::runtime_error(fmt::format("Failed to find a ResourceHandler for tag ({})", resourceFile.tag));
    }

    try {
        handler->load(m_pools, resourceFile.id, resourceFile.file);
    } catch (const std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to load resource '{}' at '{}', reason: {}\n", resourceFile.id.value(), resourceFile.file, e.what()));
    }

    DependencySolver dependencySolver;
    handler->solve_dependencies(m_pools, id, dependencySolver);

    for (const auto dependency: dependencySolver.dependencies()) {
        load(dependency);
    }

    ReferenceSolver referenceSolver(m_pools);
    handler->solve_references(m_pools, resourceFile.id, referenceSolver);
}

void ResourceSet::load(const std::string& alias) {
    auto id = find_id(alias);
    if (id == kInvalidId) {
        throw std::invalid_argument(fmt::format("resource alias '{}' not found", alias));
    }
    load(id);
}

Pools* ResourceSet::pools() const {
    return m_pools;
}

Id ResourceSet::find_id(const std::string& alias) const {
    auto it = m_resourceIdAliases.find(alias);
    if (it != m_resourceIdAliases.end()) {
        return it->second;
    }
    return kInvalidId;
}

}// namespace duk::resource
