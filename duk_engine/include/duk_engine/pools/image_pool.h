//
// Created by rov on 11/19/2023.
//

#ifndef DUK_ENGINE_IMAGE_POOL_H
#define DUK_ENGINE_IMAGE_POOL_H

#include <duk_engine/pools/pool.h>
#include <duk_renderer/renderer.h>

namespace duk::engine {

struct ImagePoolCreateInfo {
    duk::renderer::Renderer* renderer;
};

class ImagePool : public Pool<duk::rhi::Image> {
public:

    explicit ImagePool(const ImagePoolCreateInfo& imagePoolCreateInfo);

    DUK_NO_DISCARD ResourceHandle create(const duk::rhi::ImageDataSource* imageDataSource);

    DUK_NO_DISCARD ResourceHandle load(const std::string& path, duk::rhi::PixelFormat format);

    DUK_NO_DISCARD ResourceHandle white_image() const;

    DUK_NO_DISCARD ResourceHandle black_image() const;

private:
    duk::renderer::Renderer* m_renderer;
    ResourceHandle m_whiteImage;
    ResourceHandle m_blackImage;

};

}

#endif //DUK_ENGINE_IMAGE_POOL_H
