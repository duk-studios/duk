//
// Created by rov on 12/2/2023.
//

#ifndef DUK_MATERIAL_IMPORTER_H
#define DUK_MATERIAL_IMPORTER_H

#include <duk_resource/importer.h>

#include <duk_renderer/material/material_pool.h>

#include <filesystem>

namespace duk::renderer {

struct MaterialImporterCreateInfo {
    MaterialPool* materialPool;
};

class MaterialImporter : public duk::resource::ResourceImporter {
public:
    explicit MaterialImporter(const MaterialImporterCreateInfo& materialImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    MaterialPool* m_materialPool;
};

}// namespace duk::renderer

#endif//DUK_MATERIAL_IMPORTER_H
