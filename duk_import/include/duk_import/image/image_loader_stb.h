/// 12/11/2023
/// image_importer_stb.h

#ifndef DUK_IMPORT_IMAGE_IMPORTER_STB_H
#define DUK_IMPORT_IMAGE_IMPORTER_STB_H

#include <duk_import/image/image_importer.h>
#include <duk_rhi/rhi_capabilities.h>

namespace duk::import {

class ImageLoaderStb : public ResourceLoader<std::unique_ptr<duk::rhi::ImageDataSource>> {
public:
    ImageLoaderStb(const duk::rhi::RHICapabilities* rhiCapabilities);

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::rhi::ImageDataSource> load(const std::filesystem::path& path) override;

private:
    const duk::rhi::RHICapabilities* m_rhiCapabilities;
};

}// namespace duk::import

#endif// DUK_IMPORT_IMAGE_IMPORTER_STB_H
