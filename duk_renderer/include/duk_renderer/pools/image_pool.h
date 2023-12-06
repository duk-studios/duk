//
// Created by rov on 11/19/2023.
//

#ifndef DUK_RENDERER_IMAGE_POOL_H
#define DUK_RENDERER_IMAGE_POOL_H

#include <duk_pool/pool.h>
#include <duk_rhi/image.h>

namespace duk::renderer {

class Renderer;

using ImageResource = duk::pool::Resource<duk::rhi::Image>;

struct ImagePoolCreateInfo {
    Renderer* renderer;
};

class ImagePool : public duk::pool::Pool<ImageResource> {
public:

    explicit ImagePool(const ImagePoolCreateInfo& imagePoolCreateInfo);

    DUK_NO_DISCARD ImageResource create(duk::pool::ResourceId resourceId, const duk::rhi::ImageDataSource* imageDataSource);

    DUK_NO_DISCARD ImageResource white_image() const;

    DUK_NO_DISCARD ImageResource black_image() const;

private:
    duk::renderer::Renderer* m_renderer;
    ImageResource m_whiteImage;
    ImageResource m_blackImage;

};

}

#endif //DUK_RENDERER_IMAGE_POOL_H
