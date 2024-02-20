/// 13/11/2023
/// importer.cpp

#include <duk_import/importer.h>
#include <duk_import/image/image_loader_stb.h>
#include <duk_import/material/material_loader_json.h>
#include <duk_import/scene/scene_loader_json.h>
#include <duk_renderer/resources/builtin_resource_ids.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/pools/material_pool.h>
#include <duk_renderer/pools/sprite_pool.h>

namespace duk::import {

Importer::Importer(const ImporterCreateInfo& importerCreateInfo) :
    m_pools(importerCreateInfo.pools) {

    {
        ImageImporterCreateInfo imageImporterCreateInfo = {};
        imageImporterCreateInfo.rhiCapabilities = importerCreateInfo.renderer->rhi()->capabilities();
        imageImporterCreateInfo.imagePool = m_pools->get<duk::renderer::ImagePool>();
        m_resourceImporters.emplace("img", std::make_unique<ImageImporter>(imageImporterCreateInfo));
    }

    {
        MaterialImporterCreateInfo materialImporterCreateInfo = {};
        materialImporterCreateInfo.materialPool = m_pools->get<duk::renderer::MaterialPool>();
        m_resourceImporters.emplace("mat", std::make_unique<MaterialImporter>(materialImporterCreateInfo));
    }
    m_resourceImporters.emplace("scn", std::make_unique<SceneImporter>());
}

Importer::~Importer() = default;

void Importer::load_resources(const std::filesystem::path& path) {
    if (!m_resourceSetImporter.accept(path)) {
        throw std::invalid_argument("load_resources needs a valid directory path");
    }

    m_resourceSet = m_resourceSetImporter.load(path);
}

void Importer::load_resource(duk::resource::Id id) {
    if (id < duk::renderer::kMaxBuiltinResourceId) {
        // this is a builtin resource, skip
        return;
    }

    auto& resources = m_resourceSet.resources;

    auto resourceIt = resources.find(id);
    if (resourceIt == resources.end()) {
        throw std::invalid_argument("resource id not found");
    }

    const auto& resourceDescription = resourceIt->second;

    auto importerIt = m_resourceImporters.find(resourceDescription.type);
    if (importerIt == m_resourceImporters.end()) {
        throw std::runtime_error(fmt::format("Could not find an importer for resource type \"{}\"", resourceDescription.type));
    }

    auto& importer = importerIt->second;

    importer->load(resourceDescription.id, resourceDescription.path);

    duk::resource::DependencySolver dependencySolver;
    importer->solve_dependencies(resourceDescription.id, dependencySolver);

    for (auto dependency : dependencySolver.dependencies()) {
        load_resource(dependency);
    }

    duk::resource::ReferenceSolver referenceSolver(m_pools);
    importer->solve_references(resourceDescription.id, referenceSolver);
}

duk::resource::Id Importer::find_id_from_alias(const std::string& alias) {
    auto& aliases = m_resourceSet.aliases;
    auto it = aliases.find(alias);
    if (it == aliases.end()) {
        throw std::invalid_argument(fmt::format("could not find resource id for alias \"{}\"", alias));
    }
    return it->second;
}

ResourceImporter* Importer::importer_for_resource_type(const std::string& resourceType) {
    auto it = m_resourceImporters.find(resourceType);
    if (it == m_resourceImporters.end()) {
        throw std::invalid_argument(fmt::format("could not find resource importer for type \"{}\"", resourceType));
    }
    return it->second.get();
}

}