/// 17/02/2024
/// image.cpp

#include <duk_renderer/image/image.h>
#include <duk_renderer/renderer.h>

namespace duk::renderer {

Image::Image(const ImageCreateInfo& imageCreateInfo) {
    auto rhi = imageCreateInfo.rhi;
    auto commandQueue = imageCreateInfo.commandQueue;

    duk::rhi::RHI::ImageCreateInfo rhiImageCreateInfo = {};
    rhiImageCreateInfo.commandQueue = commandQueue;
    rhiImageCreateInfo.imageDataSource = imageCreateInfo.imageDataSource;
    rhiImageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::STATIC;
    rhiImageCreateInfo.usage = duk::rhi::Image::Usage::SAMPLED;
    rhiImageCreateInfo.initialLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
    rhiImageCreateInfo.dstStages = duk::rhi::PipelineStage::FRAGMENT_SHADER | duk::rhi::PipelineStage::VERTEX_SHADER | duk::rhi::PipelineStage::COMPUTE_SHADER;

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

glm::u32vec2 Image::size() const {
    return {m_image->width(), m_image->height()};
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