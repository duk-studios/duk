/// 23/04/2023
/// render_pass.h

#ifndef DUK_RHI_RENDER_PASS_H
#define DUK_RHI_RENDER_PASS_H

#include <duk_rhi/image.h>

#include <duk_macros/macros.h>

#include <cstdint>
#include <vector>

namespace duk::rhi {

enum class StoreOp {
    STORE = 0,
    DONT_CARE
};

enum class LoadOp {
    LOAD = 0,
    CLEAR,
    DONT_CARE
};

struct AttachmentDescription {
    Image::Layout initialLayout;
    Image::Layout layout;
    Image::Layout finalLayout;
    PixelFormat format;
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

}// namespace duk::rhi

namespace std {

template<>
struct hash<duk::rhi::AttachmentDescription> {
    size_t operator()(const duk::rhi::AttachmentDescription& attachmentDescription) noexcept {
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

}// namespace std

#endif// DUK_RHI_RENDER_PASS_H
