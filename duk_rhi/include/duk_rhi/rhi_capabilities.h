/// 08/06/2023
/// renderer_capabilities.h

#ifndef DUK_RHI_RENDERER_CAPABILITIES_H
#define DUK_RHI_RENDERER_CAPABILITIES_H

#include <duk_rhi/image.h>

namespace duk::rhi {

class RHICapabilities {
public:
    virtual ~RHICapabilities() = default;

    DUK_NO_DISCARD virtual PixelFormat depth_format() const = 0;

    DUK_NO_DISCARD virtual bool is_format_supported(PixelFormat format, Image::Usage usage) const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_RENDERER_CAPABILITIES_H
