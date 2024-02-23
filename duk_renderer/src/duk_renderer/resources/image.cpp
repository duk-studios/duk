/// 17/02/2024
/// image.cpp

#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/image.h>

namespace duk::renderer {

Image::Image(const ImageCreateInfo& imageCreateInfo) {
    auto renderer = imageCreateInfo.renderer;
    auto rhi = renderer->rhi();
    auto commandQueue = renderer->main_command_queue();

    duk::rhi::RHI::ImageCreateInfo rhiImageCreateInfo = {};
    rhiImageCreateInfo.commandQueue = commandQueue;
    rhiImageCreateInfo.imageDataSource = imageCreateInfo.imageDataSource;
    rhiImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::STATIC;
    rhiImageCreateInfo.usage = duk::rhi::Image::Usage::SAMPLED;
    rhiImageCreateInfo.initialLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;

    m_image = rhi->create_image(rhiImageCreateInfo);
}

duk::rhi::PixelFormat Image::format() const {
    return m_image->format();
}

uint32_t Image::width() const {
    return m_image->width();
}

uint32_t Image::height() const {
    return m_image->height();
}

duk::rhi::Image* Image::handle() {
    return m_image.get();
}

duk::rhi::Image* Image::handle() const {
    return m_image.get();
}

duk::rhi::Descriptor Image::descriptor() {
    return duk::rhi::Descriptor::image(m_image.get(), duk::rhi::Image::Layout::SHADER_READ_ONLY);
}

}// namespace duk::renderer