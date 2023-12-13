/// 13/11/2023
/// importer.h

#ifndef DUK_IMPORT_IMPORTER_H
#define DUK_IMPORT_IMPORTER_H

#include <duk_import/image/image_importer.h>
#include <duk_import/material/material_importer.h>
#include <duk_import/resource/resource_importer.h>
#include <duk_import/scene/scene_importer.h>
#include "duk_import/scene/component_json_parser.h"
#include <duk_renderer/renderer.h>
#include <duk_renderer/pools/material_pool.h>

namespace duk::import {

struct ImporterCreateInfo {
    duk::renderer::Renderer* renderer;
};

class Importer {
public:

    explicit Importer(const ImporterCreateInfo& importerCreateInfo);

    ~Importer();

    void load_resources(const std::filesystem::path& path);

    std::unique_ptr<duk::rhi::ImageDataSource> load_image_data_source(const std::filesystem::path& path);

    duk::renderer::ImageResource load_image(duk::pool::ResourceId id, const std::filesystem::path& path);

    DUK_NO_DISCARD duk::renderer::ImageResource find_image(duk::pool::ResourceId id) const;

    std::unique_ptr<duk::renderer::MaterialDataSource> load_material_data_source(const std::filesystem::path& path);

    duk::renderer::MaterialResource load_material(duk::pool::ResourceId id, const std::filesystem::path& path);

    DUK_NO_DISCARD duk::renderer::MaterialResource find_material(duk::pool::ResourceId id) const;

    DUK_NO_DISCARD std::unique_ptr<duk::scene::Scene> load_scene(duk::pool::ResourceId id);

    DUK_NO_DISCARD std::unique_ptr<duk::scene::Scene> load_scene(const std::string& alias);

    template<typename T>
    void register_component(const std::string& name) {
         m_componentParser->register_component<T>(name);
    }

private:

    void load_resource(duk::pool::ResourceId id);

private:
    duk::renderer::Renderer* m_renderer;
    std::unique_ptr<ResourceImporter> m_resourceImporter;
    std::unique_ptr<ComponentJsonParser> m_componentParser;
    std::unique_ptr<SceneImporter> m_sceneImporter;
    std::vector<std::unique_ptr<ImageImporter>> m_imageImporters;
    std::vector<std::unique_ptr<MaterialImporter>> m_materialImporters;
    ResourceSet m_resourceSet;
};

}

#endif // DUK_IMPORT_IMPORTER_H

