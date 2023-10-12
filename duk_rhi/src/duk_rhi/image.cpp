/// 23/04/2023
/// image.cpp

#include <duk_rhi/image.h>

#include <stdexcept>

namespace duk::rhi {

size_t Image::pixel_format_size(Image::PixelFormat format) {
    size_t size;
    switch (format) {
        case PixelFormat::UNDEFINED:
            size = 0;
            break;
        case PixelFormat::R8_UNORM:
        case PixelFormat::R8_SNORM:
        case PixelFormat::R8_SRGB:
            size = 1;
            break;
        case PixelFormat::R8G8_UNORM:
        case PixelFormat::R8G8_SNORM:
        case PixelFormat::R8G8_SRGB:
            size = 2;
            break;
        case PixelFormat::R8G8B8_UNORM:
        case PixelFormat::R8G8B8_SNORM:
        case PixelFormat::R8G8B8_SRGB:
        case PixelFormat::B8G8R8_UNORM:
        case PixelFormat::B8G8R8_SNORM:
        case PixelFormat::B8G8R8_SRGB:
        case PixelFormat::D16_UNORM_S8_UINT:
            size = 3;
            break;
        case PixelFormat::R8G8B8A8_UNORM:
        case PixelFormat::R8G8B8A8_SNORM:
        case PixelFormat::R8G8B8A8_SRGB:
        case PixelFormat::B8G8R8A8_UNORM:
        case PixelFormat::B8G8R8A8_SNORM:
        case PixelFormat::B8G8R8A8_SRGB:
        case PixelFormat::D32_SFLOAT:
        case PixelFormat::D24_UNORM_S8_UINT:
            size = 4;
            break;
        case PixelFormat::D32_SFLOAT_S8_UINT:
            size = 5;
            break;
        default:
            throw std::runtime_error("unhandled Image::Format on Image::pixel_format_size");
    }
    return size;
}

bool Image::is_depth_format(Image::PixelFormat format) {
    switch (format) {
        case PixelFormat::D32_SFLOAT:
        case PixelFormat::D16_UNORM_S8_UINT:
        case PixelFormat::D24_UNORM_S8_UINT:
        case PixelFormat::D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

bool Image::is_stencil_format(Image::PixelFormat format) {
    switch (format) {
        case PixelFormat::D16_UNORM_S8_UINT:
        case PixelFormat::D24_UNORM_S8_UINT:
        case PixelFormat::D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

Image::~Image() = default;

}