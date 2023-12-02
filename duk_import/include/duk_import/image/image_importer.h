/// 12/11/2023
/// image_importer.h

#ifndef DUK_IMPORT_IMAGE_IMPORTER_H
#define DUK_IMPORT_IMAGE_IMPORTER_H

#include <duk_rhi/image_data_source.h>

#include <filesystem>

namespace duk::import {

class ImageImporter {
public:

    virtual ~ImageImporter();

    virtual bool accepts(const std::filesystem::path& path, duk::rhi::PixelFormat desiredPixelFormat) = 0;

    virtual std::unique_ptr<duk::rhi::ImageDataSource> load(const std::filesystem::path& path, duk::rhi::PixelFormat desiredPixelFormat) = 0;

    static std::unique_ptr<duk::rhi::ImageDataSource> create(const void* data, duk::rhi::PixelFormat format, uint32_t width, uint32_t height);

};

}

#endif // DUK_IMPORT_IMAGE_IMPORTER_H

