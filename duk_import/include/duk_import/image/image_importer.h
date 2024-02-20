/// 12/11/2023
/// image_importer.h

#ifndef DUK_IMPORT_IMAGE_IMPORTER_H
#define DUK_IMPORT_IMAGE_IMPORTER_H

#include <duk_rhi/image_data_source.h>
#include <duk_import/resource_importer.h>
#include <duk_renderer/renderer.h>

#include <filesystem>

namespace duk::import {

struct ImageImporterCreateInfo {
    duk::rhi::RHICapabilities* rhiCapabilities;
    duk::renderer::ImagePool* imagePool;
};

class ImageImporter : public ResourceImporterT<duk::rhi::ImageDataSource> {
public:

    static std::unique_ptr<duk::rhi::ImageDataSource> create(const void* data, duk::rhi::PixelFormat format, uint32_t width, uint32_t height);

    explicit ImageImporter(const ImageImporterCreateInfo& imageImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

private:
    duk::renderer::ImagePool* m_imagePool;
};

}

#endif // DUK_IMPORT_IMAGE_IMPORTER_H

