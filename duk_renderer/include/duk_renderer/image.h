/// 23/04/2023
/// image.h

#ifndef DUK_RENDERER_IMAGE_H
#define DUK_RENDERER_IMAGE_H

#include <duk_macros/macros.h>

namespace duk::renderer {

enum class ImageLayout {
    UNDEFINED = 0,
    GENERAL,
    COLOR_ATTACHMENT,
    DEPTH_ATTACHMENT,
    SHADER_READ_ONLY,
    PRESENT_SRC,
};

enum class ImageFormat {
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
};

class Image {
public:
    virtual ~Image() = default;

    DUK_NO_DISCARD virtual ImageFormat format() const = 0;

};

}

#endif // DUK_RENDERER_IMAGE_H

