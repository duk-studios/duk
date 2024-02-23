//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RENDERER_IMAGE_POOL_H
#define DUK_RENDERER_IMAGE_POOL_H

#include <duk_renderer/resources/image.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

// 1 - 10,000 - reserved for built-in images
static constexpr duk::resource::Id kWhiteImageId(1);
static constexpr duk::resource::Id kBlackImageId(2);

class Renderer;

struct ImagePoolCreateInfo {
    Renderer* renderer;
};

class ImagePool : public duk::resource::PoolT<ImageResource> {
public:
    explicit ImagePool(const ImagePoolCreateInfo& imagePoolCreateInfo);

    ImageResource create(duk::resource::Id resourceId, const duk::rhi::ImageDataSource* imageDataSource);

    DUK_NO_DISCARD ImageResource white_image() const;

    DUK_NO_DISCARD ImageResource black_image() const;

private:
    duk::renderer::Renderer* m_renderer;
    ImageResource m_whiteImage;
    ImageResource m_blackImage;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_IMAGE_POOL_H
