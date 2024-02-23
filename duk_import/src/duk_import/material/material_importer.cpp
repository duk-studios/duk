//
// Created by rov on 12/2/2023.
//

#include <duk_import/material/material_importer.h>
#include <duk_import/material/material_loader_json.h>

namespace duk::import {

MaterialImporter::MaterialImporter(const MaterialImporterCreateInfo& materialImporterCreateInfo)
    : m_materialPool(materialImporterCreateInfo.materialPool) {
    add_loader<MaterialLoaderJson>();
}

const std::string& MaterialImporter::tag() const {
    static const std::string matTag("mat");
    return matTag;
}

void MaterialImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    auto dataSource = ResourceImporterT<std::unique_ptr<duk::renderer::MaterialDataSource>>::load(path);
    m_materialPool->create(id, dataSource.get());
}

void MaterialImporter::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto material = m_materialPool->find(id);
    if (!material) {
        throw std::logic_error("tried to solve dependencies of resource that was not loaded");
    }
    dependencySolver.solve(*material);
}

void MaterialImporter::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto material = m_materialPool->find(id);
    if (!material) {
        throw std::logic_error("tried to solve references of resource that was not loaded");
    }
    referenceSolver.solve(*material);
}

}// namespace duk::import