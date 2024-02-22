/// 17/02/2024
/// image.h

#ifndef DUK_RENDERER_IMAGE_H
#define DUK_RENDERER_IMAGE_H

#include <duk_resource/resource.h>
#include <duk_rhi/image.h>
#include <duk_rhi/descriptor.h>

namespace duk::renderer {

class Renderer;

struct ImageCreateInfo {
    Renderer* renderer;
    const duk::rhi::ImageDataSource* imageDataSource;
};

class Image  {
public:

    explicit Image(const ImageCreateInfo& imageCreateInfo);

    DUK_NO_DISCARD duk::rhi::PixelFormat format() const;

    DUK_NO_DISCARD uint32_t width() const;

    DUK_NO_DISCARD uint32_t height() const;

    DUK_NO_DISCARD duk::rhi::Image* handle();

    DUK_NO_DISCARD duk::rhi::Image* handle() const;

    DUK_NO_DISCARD duk::rhi::Descriptor descriptor();

private:
    std::shared_ptr<duk::rhi::Image> m_image;
};

using ImageResource = duk::resource::ResourceT<Image>;

}

#endif // DUK_RENDERER_IMAGE_H

