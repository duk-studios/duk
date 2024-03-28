/// 12/11/2023
/// image_importer_stb.h

#ifndef DUK_RENDERER_IMAGE_IMPORTER_STB_H
#define DUK_RENDERER_IMAGE_IMPORTER_STB_H

#include <duk_renderer/resources/image_importer.h>

#include <duk_rhi/rhi_capabilities.h>

namespace duk::renderer {

class ImageLoaderStb : public ImageLoader {
public:
    ImageLoaderStb(const duk::rhi::RHICapabilities* rhiCapabilities);

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::rhi::ImageDataSource> load(const std::filesystem::path& path) override;

private:
    const duk::rhi::RHICapabilities* m_rhiCapabilities;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_IMAGE_IMPORTER_STB_H
