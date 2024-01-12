/// 08/06/2023
/// renderer_capabilities.h

#ifndef DUK_RHI_RENDERER_CAPABILITIES_H
#define DUK_RHI_RENDERER_CAPABILITIES_H

#include <duk_rhi/image.h>

namespace duk::rhi {

class RendererCapabilities {
public:

    virtual ~RendererCapabilities() = default;

    virtual PixelFormat depth_format() = 0;

    virtual bool is_format_supported(PixelFormat format, Image::Usage usage) = 0;

};

}

#endif // DUK_RHI_RENDERER_CAPABILITIES_H

