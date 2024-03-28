/// 12/11/2023
/// image_importer.h

#ifndef DUK_RENDERER_IMAGE_IMPORTER_H
#define DUK_RENDERER_IMAGE_IMPORTER_H

#include <duk_resource/importer.h>

#include <duk_renderer/renderer.h>

#include <duk_rhi/image_data_source.h>

#include <filesystem>

namespace duk::renderer {

class ImageLoader {
public:
    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual std::unique_ptr<duk::rhi::ImageDataSource> load(const std::filesystem::path& path) = 0;
};

struct ImageImporterCreateInfo {
    duk::rhi::RHICapabilities* rhiCapabilities;
    ImagePool* imagePool;
};

class ImageImporter : public duk::resource::ResourceImporter {
public:
    static std::unique_ptr<duk::rhi::ImageDataSource> create(const void* data, duk::rhi::PixelFormat format, uint32_t width, uint32_t height);

    explicit ImageImporter(const ImageImporterCreateInfo& imageImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

private:
    std::vector<std::unique_ptr<ImageLoader>> m_loaders;
    ImagePool* m_imagePool;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_IMAGE_IMPORTER_H
