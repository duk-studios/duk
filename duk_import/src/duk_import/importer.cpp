/// 13/11/2023
/// importer.cpp

#include <duk_import/importer.h>
#include <duk_import/image/image_importer_stb.h>
#include <duk_import/material/material_importer_json.h>
#include <duk_import/resource/resource_importer_json.h>

namespace duk::import {

Importer::Importer(const ImporterCreateInfo& importerCreateInfo) :
    m_renderer(importerCreateInfo.renderer) {

    auto imagePool = m_renderer->image_pool();

    m_imageImporters.emplace_back(std::make_unique<ImageImporterStb>());
    m_materialImporters.emplace_back(std::make_unique<MaterialImporterJson>(imagePool));
    m_resourceImporter = std::make_unique<ResourceImporterJson>();
}

Importer::~Importer() = default;

void Importer::load_resources(const std::filesystem::path& path) {
    if (!m_resourceImporter->accept(path)) {
        throw std::invalid_argument("invalid resource file path");
    }

    auto resourceSet = m_resourceImporter->load(path);

    for (auto& imageDescription : resourceSet.images) {
        load_image(imageDescription.id, imageDescription.path);
    }

    for (auto& materialDescription : resourceSet.materials) {
        load_material(materialDescription.id, materialDescription.path);
    }
}

std::unique_ptr<duk::rhi::ImageDataSource> Importer::load_image_data_source(const std::filesystem::path& path) {
    for (auto& importer : m_imageImporters) {
        if (importer->accepts(path)) {
            return importer->load(path);
        }
    }
    throw std::runtime_error("failed to load image data source: could not find suitable importer");
}

duk::renderer::ImageResource Importer::load_image(duk::pool::ResourceId resourceId, const std::filesystem::path& path) {
    auto dataSource = load_image_data_source(path);
    return m_renderer->image_pool()->create(resourceId, dataSource.get());
}

duk::renderer::ImageResource Importer::find_image(duk::pool::ResourceId resourceId) {
    return m_renderer->image_pool()->find(resourceId);
}

std::unique_ptr<duk::renderer::MaterialDataSource> Importer::load_material_data_source(const std::filesystem::path& path) {
    for (auto& importer : m_materialImporters) {
        if (importer->accepts(path)) {
            return importer->load(path);
        }
    }
    throw std::runtime_error("failed to load material: could not find suitable importer");
}

duk::renderer::MaterialResource Importer::load_material(duk::pool::ResourceId resourceId, const std::filesystem::path& path) {
    auto dataSource = load_material_data_source(path);
    return  m_renderer->material_pool()->create(resourceId, dataSource.get());
}

duk::renderer::MaterialResource Importer::find_material(duk::pool::ResourceId resourceId) {
    return m_renderer->material_pool()->find(resourceId);
}



}