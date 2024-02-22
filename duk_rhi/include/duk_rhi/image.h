/// 23/04/2023
/// image.h

#ifndef DUK_RHI_IMAGE_H
#define DUK_RHI_IMAGE_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>
#include <duk_rhi/pixel.h>

#include <cstdint>

namespace duk::rhi {

class ImageDataSource;

class Image {
public:
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

    static constexpr SubresourceRange kFullSubresourceRange{
            0u, SubresourceRange::kRemainingMipLevels,
            0u, SubresourceRange::kRemainingArrayLayers};

public:
    virtual ~Image();

    virtual void update(ImageDataSource* imageDataSource) = 0;

    DUK_NO_DISCARD virtual PixelFormat format() const = 0;

    DUK_NO_DISCARD virtual uint32_t width() const = 0;

    DUK_NO_DISCARD virtual uint32_t height() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_IMAGE_H
