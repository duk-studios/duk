//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RENDERER_IMAGE_BUILTINS_H
#define DUK_RENDERER_IMAGE_BUILTINS_H

#include <duk_renderer/image/image.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

// 1 - 10,000 - reserved for built-in images
static constexpr duk::resource::Id kWhiteImageId(1);
static constexpr duk::resource::Id kBlackImageId(2);

class Renderer;

struct ImageBuiltinsCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    duk::resource::Pools* pools;
};

class ImageBuiltins {
public:
    explicit ImageBuiltins(const ImageBuiltinsCreateInfo& imageBuiltinsCreateInfo);

    DUK_NO_DISCARD duk::resource::Handle<Image> white() const;

    DUK_NO_DISCARD duk::resource::Handle<Image> black() const;

private:
    duk::resource::Handle<Image> m_white;
    duk::resource::Handle<Image> m_black;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_IMAGE_BUILTINS_H
