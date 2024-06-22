/// 12/11/2023
/// image_handler.h

#ifndef DUK_RENDERER_IMAGE_HANDLER_H
#define DUK_RENDERER_IMAGE_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_renderer/image/image_pool.h>
#include <duk_renderer/renderer.h>

#include <duk_rhi/image_data_source.h>

#include <filesystem>

namespace duk::renderer {

class ImageLoader {
public:
    virtual ~ImageLoader() = default;

    virtual bool accepts(const std::string_view& extension) = 0;

    virtual bool accepts(const void* data, size_t size) = 0;

    virtual std::unique_ptr<duk::rhi::ImageDataSource> load(const void* data, size_t size) = 0;
};

class ImageHandler : public duk::resource::ResourceHandlerT<ImagePool> {
public:
    static std::unique_ptr<duk::rhi::ImageDataSource> create(const void* data, duk::rhi::PixelFormat format, uint32_t width, uint32_t height);

    ImageHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<Image> load_from_memory(ImagePool* pool, const resource::Id& id, const void* data, size_t size) override;

private:
    std::vector<std::unique_ptr<ImageLoader>> m_loaders;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_IMAGE_HANDLER_H
