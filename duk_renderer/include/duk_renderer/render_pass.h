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

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;


};

}

namespace std {

template<>
struct hash<duk::renderer::AttachmentDescription> {
    size_t operator()(const duk::renderer::AttachmentDescription& attachmentDescription) noexcept {
        size_t hash = 0;
        duk::hash::hash_combine(hash, attachmentDescription.initialLayout);
        duk::hash::hash_combine(hash, attachmentDescription.layout);
        duk::hash::hash_combine(hash, attachmentDescription.finalLayout);
        duk::hash::hash_combine(hash, attachmentDescription.format);
        duk::hash::hash_combine(hash, attachmentDescription.storeOp);
        duk::hash::hash_combine(hash, attachmentDescription.loadOp);
        return hash;
    }
};

}

#endif // DUK_RENDERER_RENDER_PASS_H

