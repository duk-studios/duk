/// 23/04/2023
/// image.h

#ifndef DUK_RHI_IMAGE_H
#define DUK_RHI_IMAGE_H

#include <duk_rhi/resource.h>
#include <duk_macros/macros.h>

#include <cstdint>

namespace duk::rhi {

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
        SAMPLED_STORAGE,
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT
    };

    enum class UpdateFrequency {
        STATIC,
        HOST_DYNAMIC,
        DEVICE_DYNAMIC
    };

    struct SubresourceRange {
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;
        static constexpr uint32_t kRemainingMipLevels = ~0u;
        static constexpr uint32_t kRemainingArrayLayers = ~0u;
    };

    static constexpr SubresourceRange kFullSubresourceRange {
        0u, SubresourceRange::kRemainingMipLevels,
        0u, SubresourceRange::kRemainingArrayLayers
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

#endif // DUK_RHI_IMAGE_H

