/// 23/04/2023
/// image.h

#ifndef DUK_RENDERER_IMAGE_H
#define DUK_RENDERER_IMAGE_H

#include <duk_renderer/resource.h>
#include <duk_macros/macros.h>

#include <cstdint>

namespace duk::renderer {

class ImageDataSource;

class Image : public Resource {
public:
    enum class PixelFormat {
        UNDEFINED = 0,
        R8_UNORM,
        R8_SNORM,
        R8_SRGB,
        R8G8_UNORM,
        R8G8_SNORM,
        R8G8_SRGB,
        R8G8B8_UNORM,
        R8G8B8_SNORM,
        R8G8B8_SRGB,
        B8G8R8_UNORM,
        B8G8R8_SNORM,
        B8G8R8_SRGB,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_SRGB,
        B8G8R8A8_UNORM,
        B8G8R8A8_SNORM,
        B8G8R8A8_SRGB,
        D32_SFLOAT,
        D16_UNORM_S8_UINT,
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT
    };

    enum class Layout {
        UNDEFINED = 0,
        GENERAL,
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        SHADER_READ_ONLY,
        PRESENT_SRC
    };

    enum class Usage {
        SAMPLED,
        STORAGE,
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT
    };

    enum class UpdateFrequency {
        STATIC,
        HOST_DYNAMIC,
        DEVICE_DYNAMIC
    };

public:

    virtual void update(ImageDataSource* imageDataSource) = 0;

    DUK_NO_DISCARD virtual PixelFormat format() const = 0;

    DUK_NO_DISCARD virtual uint32_t width() const = 0;

    DUK_NO_DISCARD virtual uint32_t height() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;

    DUK_NO_DISCARD static size_t pixel_format_size(PixelFormat format);

    DUK_NO_DISCARD static bool is_depth_format(PixelFormat format);

    DUK_NO_DISCARD static bool is_stencil_format(PixelFormat format);

};

}

#endif // DUK_RENDERER_IMAGE_H

