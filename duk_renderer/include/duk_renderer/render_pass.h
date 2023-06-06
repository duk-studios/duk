/// 23/04/2023
/// render_pass.h

#ifndef DUK_RENDERER_RENDER_PASS_H
#define DUK_RENDERER_RENDER_PASS_H

#include <duk_renderer/image.h>

#include <duk_macros/macros.h>

#include <vector>
#include <cstdint>

namespace duk::renderer {

enum class StoreOp {
    STORE = 0,
    DONT_CARE,
    NONE,
};

enum class LoadOp {
    LOAD = 0,
    CLEAR,
    DONT_CARE,
    NONE,
};

struct AttachmentDescription {
    Image::Layout initialLayout;
    Image::Layout layout;
    Image::Layout finalLayout;
    Image::PixelFormat format;
    StoreOp storeOp;
    LoadOp loadOp;
};

class RenderPass {
public:

    virtual ~RenderPass();

    DUK_NO_DISCARD virtual size_t attachment_count() const = 0;

    DUK_NO_DISCARD virtual size_t color_attachment_count() const = 0;

    DUK_NO_DISCARD virtual bool has_depth_attachment() const = 0;


};

}

#endif // DUK_RENDERER_RENDER_PASS_H

