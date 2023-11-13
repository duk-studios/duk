/// 12/11/2023
/// image_importer_stb.cpp

#include <duk_import/image/image_importer_stb.h>
#include <stb_image.h>
#include <stdexcept>

namespace duk::import {

bool ImageImporterStb::accepts(const std::filesystem::path& path, duk::rhi::PixelFormat desiredPixelFormat) {
    const auto extension = path.extension().string();
    if (extension != ".png" && extension != ".jpg" && extension != ".jpeg") {
        return false;
    }

    const bool isUnsignedInt8 = desiredPixelFormat & duk::rhi::PixelFlags::UINT8;
    const bool isUnsignedInt16 = desiredPixelFormat & duk::rhi::PixelFlags::UINT16;
    const bool isFloat = desiredPixelFormat & duk::rhi::PixelFlags::FLOAT32;

    if (!isUnsignedInt8 && !isUnsignedInt16 && !isFloat) {
        return false;
    }

    return true;
}

std::unique_ptr<duk::rhi::ImageDataSource> ImageImporterStb::load(const std::filesystem::path& path, duk::rhi::PixelFormat desiredPixelFormat) {

    const auto pathStr = path.string();
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("failed to load image: file does not exist: " + pathStr);
    }

    void* data;
    int width, height, channelCount;
    int desiredChannels = desiredPixelFormat.channel_count();

    if (desiredPixelFormat & duk::rhi::PixelFlags::UINT8) {
        data = stbi_load(pathStr.c_str(), &width, &height, &channelCount, desiredChannels);
    }
    else if (desiredPixelFormat & duk::rhi::PixelFlags::UINT16) {
        data = stbi_load_16(pathStr.c_str(), &width, &height, &channelCount, desiredChannels);
    }
    else if (desiredPixelFormat & duk::rhi::PixelFlags::FLOAT32) {
        data = stbi_loadf(pathStr.c_str(), &width, &height, &channelCount, desiredChannels);
    }
    else {
        throw std::invalid_argument("failed to load image: unsupported image format");
    }

    if (!data) {
        throw std::runtime_error("failed to load image: failed to decode image: " + std::string(stbi_failure_reason()));
    }

    auto image = ImageImporter::create(data, desiredPixelFormat, width, height);

    image->update_hash();

    stbi_image_free(data);

    return image;
}

}
