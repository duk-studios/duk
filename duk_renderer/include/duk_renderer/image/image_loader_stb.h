/// 12/11/2023
/// image_handler_stb.h

#ifndef DUK_RENDERER_IMAGE_LOADER_STB_H
#define DUK_RENDERER_IMAGE_LOADER_STB_H

#include <duk_renderer/image/image_handler.h>

namespace duk::renderer {

class ImageLoaderStb : public ImageLoader {
public:
    bool accepts(const std::string_view& extension) override;

    bool accepts(const void* data, size_t size) override;

    std::unique_ptr<duk::rhi::ImageDataSource> load(const void* data, size_t size) override;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_IMAGE_LOADER_STB_H
