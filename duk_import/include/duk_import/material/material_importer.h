//
// Created by rov on 12/2/2023.
//

#ifndef DUK_MATERIAL_IMPORTER_H
#define DUK_MATERIAL_IMPORTER_H

#include <duk_import/resource_importer.h>
#include <duk_renderer/pools/material_pool.h>

#include <filesystem>

namespace duk::import {

struct MaterialImporterCreateInfo {
    duk::renderer::MaterialPool* materialPool;
};

class MaterialImporter : public ResourceImporterT<std::unique_ptr<duk::renderer::MaterialDataSource>> {
public:
    explicit MaterialImporter(const MaterialImporterCreateInfo& materialImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    duk::renderer::MaterialPool* m_materialPool;
};

}// namespace duk::import

#endif//DUK_MATERIAL_IMPORTER_H
