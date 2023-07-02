/// 08/06/2023
/// renderer_capabilities.h

#ifndef DUK_RHI_RENDERER_CAPABILITIES_H
#define DUK_RHI_RENDERER_CAPABILITIES_H

#include <duk_rhi/image.h>

namespace duk::rhi {

class RendererCapabilities {
public:

    virtual ~RendererCapabilities() = default;

    virtual Image::PixelFormat depth_format() = 0;
};

}

#endif // DUK_RHI_RENDERER_CAPABILITIES_H

