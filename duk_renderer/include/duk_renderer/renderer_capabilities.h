/// 08/06/2023
/// renderer_capabilities.h

#ifndef DUK_RENDERER_RENDERER_CAPABILITIES_H
#define DUK_RENDERER_RENDERER_CAPABILITIES_H

#include <duk_renderer/image.h>

namespace duk::renderer {

class RendererCapabilities {
public:

    virtual ~RendererCapabilities() = default;

    virtual Image::PixelFormat depth_format() = 0;
};

}

#endif // DUK_RENDERER_RENDERER_CAPABILITIES_H

