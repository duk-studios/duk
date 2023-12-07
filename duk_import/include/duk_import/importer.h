/// 13/11/2023
/// importer.h

#ifndef DUK_IMPORT_IMPORTER_H
#define DUK_IMPORT_IMPORTER_H

#include <duk_import/image/image_importer.h>
#include <duk_import/material/material_importer.h>
#include <duk_import/resource/resource_importer.h>
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

    duk::renderer::ImageResource load_image(duk::pool::ResourceId resourceId, const std::filesystem::path& path);

    duk::renderer::ImageResource find_image(duk::pool::ResourceId resourceId);

    std::unique_ptr<duk::renderer::MaterialDataSource> load_material_data_source(const std::filesystem::path& path);

    duk::renderer::MaterialResource load_material(duk::pool::ResourceId resourceId, const std::filesystem::path& path);

    duk::renderer::MaterialResource find_material(duk::pool::ResourceId resourceId);



private:
    duk::renderer::Renderer* m_renderer;
    std::unique_ptr<ResourceImporter> m_resourceImporter;
    std::vector<std::unique_ptr<ImageImporter>> m_imageImporters;
    std::vector<std::unique_ptr<MaterialImporter>> m_materialImporters;
};

}

#endif // DUK_IMPORT_IMPORTER_H

