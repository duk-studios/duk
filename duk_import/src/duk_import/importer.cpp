/// 13/11/2023
/// importer.cpp

#include <duk_import/importer.h>
#include <duk_import/scene/scene_loader_json.h>

namespace duk::import {

Importer::Importer(const ImporterCreateInfo& importerCreateInfo)
    : m_pools(importerCreateInfo.pools) {
}

Importer::~Importer() = default;

ResourceImporter* Importer::add_resource_importer(std::unique_ptr<ResourceImporter> resourceImporter) {
    const auto& tag = resourceImporter->tag();
    auto it = m_resourceImporters.find(tag);
    if (it != m_resourceImporters.end()) {
        throw std::logic_error(fmt::format("a ResourceImporter with tag \"{}\" already exists in this importer", tag));
    }
    return m_resourceImporters.emplace(tag, std::move(resourceImporter)).first->second.get();
}

void Importer::load_resource_set(const std::filesystem::path& path) {
    std::filesystem::path directory = path;
    if (!std::filesystem::is_directory(path)) {
        directory = path.parent_path();
    }

    if (!m_resourceSetImporter.accept(directory)) {
        throw std::invalid_argument("load_resource_set needs a valid directory path");
    }

    m_resourceSet = m_resourceSetImporter.load(directory);
}

void Importer::load_resource(duk::resource::Id id) {
    if (id < kMaxBuiltInResourceId) {
        // this is a builtin resource, skip
        return;
    }

    auto& resources = m_resourceSet.resources;

    auto resourceIt = resources.find(id);
    if (resourceIt == resources.end()) {
        throw std::invalid_argument("resource id not found");
    }

    const auto& resourceDescription = resourceIt->second;

    auto importer = get_importer(resourceDescription.tag);
    if (!importer) {
        throw std::runtime_error(fmt::format("Could not find an importer for tag \"{}\"", resourceDescription.tag));
    }

    importer->load(resourceDescription.id, resourceDescription.path);

    duk::resource::DependencySolver dependencySolver;
    importer->solve_dependencies(resourceDescription.id, dependencySolver);

    for (auto dependency: dependencySolver.dependencies()) {
        load_resource(dependency);
    }

    duk::resource::ReferenceSolver referenceSolver(m_pools);
    importer->solve_references(resourceDescription.id, referenceSolver);
}

duk::resource::Id Importer::find_id(const std::string& alias) {
    auto& aliases = m_resourceSet.aliases;
    auto it = aliases.find(alias);
    if (it == aliases.end()) {
        throw std::invalid_argument(fmt::format("could not find resource id for alias \"{}\"", alias));
    }
    return it->second;
}

ResourceImporter* Importer::get_importer(const std::string& tag) {
    auto it = m_resourceImporters.find(tag);
    if (it == m_resourceImporters.end()) {
        return nullptr;
    }
    return it->second.get();
}

}// namespace duk::import