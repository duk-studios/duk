/// 12/11/2023
/// image_handler_stb.cpp

#include <duk_renderer/image/image_loader_stb.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <stdexcept>

namespace duk::renderer {

namespace detail {

duk::rhi::PixelFormat build_pixel_format(int channelCount) {
    switch (channelCount) {
        case 1:
            return duk::rhi::PixelFormat::R8U;
        case 2:
            return duk::rhi::PixelFormat::RG8U;
        case 3:
            return duk::rhi::PixelFormat::RGB8U;
        case 4:
            return duk::rhi::PixelFormat::RGBA8U;
        default:
            throw std::invalid_argument("unsupported channel count");
    }
}

}// namespace detail

bool ImageLoaderStb::accepts(const std::filesystem::path& extension) {
    if (extension != ".png" && extension != ".jpg" && extension != ".jpeg") {
        return false;
    }
    return true;
}

std::unique_ptr<duk::rhi::ImageDataSource> ImageLoaderStb::load(const std::filesystem::path& path) {
    const auto pathStr = path.string();
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("file does not exist: " + pathStr);
    }

    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("failed to open file");
    }
    auto size = file.tellg();
    file.seekg(0);

    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

    int width, height, channelCount;
    if (!stbi_info_from_memory(buffer.data(), static_cast<int>(buffer.size()), &width, &height, &channelCount)) {
        throw std::runtime_error("failed to read image memory");
    }

    // for now we simply treat every RGB image as RGBA.
    // once the RHI is able to detect incompatibility and
    // correct them as needed we can remove this
    if (channelCount == 3) {
        channelCount = 4;
    }

    auto format = detail::build_pixel_format(channelCount);

    void* data = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size()), &width, &height, &channelCount, channelCount);

    if (!data) {
        throw std::runtime_error("failed to decode image: " + std::string(stbi_failure_reason()));
    }

    auto image = ImageHandler::create(data, format, width, height);

    image->update_hash();

    stbi_image_free(data);

    return image;
}

}// namespace duk::renderer
