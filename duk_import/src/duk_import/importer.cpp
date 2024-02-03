/// 13/11/2023
/// importer.cpp

#include <duk_import/importer.h>
#include <duk_import/image/image_importer_stb.h>
#include <duk_import/material/material_importer_json.h>
#include <duk_import/resource/resource_importer_json.h>
#include <duk_import/scene/scene_importer_json.h>

namespace duk::import {

Importer::Importer(const ImporterCreateInfo& importerCreateInfo) :
    m_renderer(importerCreateInfo.renderer) {

    auto imagePool = m_renderer->image_pool();

    m_imageImporters.emplace_back(std::make_unique<ImageImporterStb>(m_renderer->rhi()->capabilities()));
    m_materialImporters.emplace_back(std::make_unique<MaterialImporterJson>(imagePool));
    m_resourceImporter = std::make_unique<ResourceImporterJson>();

    {
        SceneImporterJsonCreateInfo sceneImporterJsonCreateInfo = {};
        sceneImporterJsonCreateInfo.componentBuilder = importerCreateInfo.componentBuilder;

        m_sceneImporter = std::make_unique<SceneImporterJson>(sceneImporterJsonCreateInfo);
    }
}

Importer::~Importer() = default;

void Importer::load_resources(const std::filesystem::path& path) {
    if (!m_resourceImporter->accept(path)) {
        throw std::invalid_argument("invalid resource file path");
    }

    m_resourceSet = m_resourceImporter->load(path);
}

std::unique_ptr<duk::rhi::ImageDataSource> Importer::load_image_data_source(const std::filesystem::path& path) {
    for (auto& importer : m_imageImporters) {
        if (importer->accepts(path)) {
            return importer->load(path);
        }
    }
    throw std::runtime_error("failed to load image data source: could not find suitable importer");
}

duk::renderer::ImageResource Importer::load_image(duk::pool::ResourceId id, const std::filesystem::path& path) {
    if (auto img = find_image(id)) {
        return img;
    }
    auto dataSource = load_image_data_source(path);
    return m_renderer->image_pool()->create(id, dataSource.get());
}

duk::renderer::ImageResource Importer::find_image(duk::pool::ResourceId id) const {
    return m_renderer->image_pool()->find(id);
}

std::unique_ptr<duk::renderer::MaterialDataSource> Importer::load_material_data_source(const std::filesystem::path& path) {
    for (auto& importer : m_materialImporters) {
        if (importer->accepts(path)) {
            return importer->load(path);
        }
    }
    throw std::runtime_error("failed to load material: could not find suitable importer");
}

duk::renderer::MaterialResource Importer::load_material(duk::pool::ResourceId id, const std::filesystem::path& path) {
    if (auto mat = find_material(id)) {
        return mat;
    }
    auto dataSource = load_material_data_source(path);
    return  m_renderer->material_pool()->create(id, dataSource.get());
}

duk::renderer::MaterialResource Importer::find_material(duk::pool::ResourceId id) const {
    return m_renderer->material_pool()->find(id);
}

std::unique_ptr<duk::scene::Scene> Importer::load_scene(duk::pool::ResourceId id) {

    auto& resources = m_resourceSet.resources;

    auto it = resources.find(id);
    if (it == resources.end()) {
        throw std::invalid_argument("scene id not found");
    }

    // load dependencies
    const auto& sceneDescription = it->second;
    if (sceneDescription.type != ResourceType::SCN) {
        throw std::invalid_argument("scene id does not correspond to a scene resource");
    }

    for (const auto& dependencyId : sceneDescription.dependencies) {
        load_resource(dependencyId);
    }

    return m_sceneImporter->load(sceneDescription.path);
}

std::unique_ptr<duk::scene::Scene> Importer::load_scene(const std::string& alias) {

    auto& aliases = m_resourceSet.aliases;
    auto it = aliases.find(alias);
    if (it == aliases.end()) {
        throw std::invalid_argument("alias \"" + alias + "\" was not found");
    }

    return load_scene(it->second);
}

void Importer::load_resource(duk::pool::ResourceId id) {
    auto& resources = m_resourceSet.resources;

    auto it = resources.find(id);
    if (it == resources.end()) {
        throw std::invalid_argument("resource id not found");
    }

    const auto& resourceDescription = it->second;

    for (const auto& dependencyId : resourceDescription.dependencies) {
        load_resource(dependencyId);
    }

    switch (resourceDescription.type) {
        case ResourceType::IMG:
            load_image(resourceDescription.id, resourceDescription.path);
            break;
        case ResourceType::MAT:
            load_material(resourceDescription.id, resourceDescription.path);
            break;
        default:
            throw std::logic_error("loading unsupported resource type");
    }

}

}