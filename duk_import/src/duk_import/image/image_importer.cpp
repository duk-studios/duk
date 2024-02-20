/// 12/11/2023
/// image_importer.cpp

#include <duk_import/image/image_importer.h>
#include <duk_import/image/image_loader_stb.h>
#include <duk_renderer/pools/image_pool.h>

namespace duk::import {

std::unique_ptr<duk::rhi::ImageDataSource> ImageImporter::create(const void* data, duk::rhi::PixelFormat format, uint32_t width, uint32_t height) {
    switch (format) {
        case duk::rhi::PixelFormat::R8S:
            return std::make_unique<duk::rhi::ImageDataSourceR8S>(reinterpret_cast<const duk::rhi::PixelR8S*>(data), width, height);
        case duk::rhi::PixelFormat::R8U:
            return std::make_unique<duk::rhi::ImageDataSourceR8U>(reinterpret_cast<const duk::rhi::PixelR8U*>(data), width, height);
        case duk::rhi::PixelFormat::RG8S:
            return std::make_unique<duk::rhi::ImageDataSourceRG8S>(reinterpret_cast<const duk::rhi::PixelRG8S*>(data), width, height);
        case duk::rhi::PixelFormat::RG8U:
            return std::make_unique<duk::rhi::ImageDataSourceRG8U>(reinterpret_cast<const duk::rhi::PixelRG8U*>(data), width, height);
        case duk::rhi::PixelFormat::RGB8S:
            return std::make_unique<duk::rhi::ImageDataSourceRGB8S>(reinterpret_cast<const duk::rhi::PixelRGB8S*>(data), width, height);
        case duk::rhi::PixelFormat::RGB8U:
            return std::make_unique<duk::rhi::ImageDataSourceRGB8U>(reinterpret_cast<const duk::rhi::PixelRGB8U*>(data), width, height);
        case duk::rhi::PixelFormat::RGBA8S:
            return std::make_unique<duk::rhi::ImageDataSourceRGBA8S>(reinterpret_cast<const duk::rhi::PixelRGBA8S*>(data), width, height);
        case duk::rhi::PixelFormat::RGBA8U:
            return std::make_unique<duk::rhi::ImageDataSourceRGBA8U>(reinterpret_cast<const duk::rhi::PixelRGBA8U*>(data), width, height);
        case duk::rhi::PixelFormat::RGBA32S:
            return std::make_unique<duk::rhi::ImageDataSourceRGBA32S>(reinterpret_cast<const duk::rhi::PixelRGBA32S*>(data), width, height);
        case duk::rhi::PixelFormat::RGBA32U:
            return std::make_unique<duk::rhi::ImageDataSourceRGBA32U>(reinterpret_cast<const duk::rhi::PixelRGBA32U*>(data), width, height);
        case duk::rhi::PixelFormat::RGBA32F:
            return std::make_unique<duk::rhi::ImageDataSourceRGBA32F>(reinterpret_cast<const duk::rhi::PixelRGBA32F*>(data), width, height);
        default:
            throw std::invalid_argument("failed to create Image: invalid PixelFormat");
    }
}

ImageImporter::ImageImporter(const ImageImporterCreateInfo& imageImporterCreateInfo) :
    m_imagePool(imageImporterCreateInfo.imagePool) {
    add_loader<ImageLoaderStb>(imageImporterCreateInfo.rhiCapabilities);
}

void ImageImporter::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    auto dataSource = ResourceImporterT<duk::rhi::ImageDataSource>::load(path);
    m_imagePool->create(id, dataSource.get());
}

}
