//
// Created by rov on 11/19/2023.
//

#include <duk_engine/pools/image_pool.h>
#include <duk_import/importer.h>

namespace duk::engine {

ImagePool::ImagePool(const ImagePoolCreateInfo& imagePoolCreateInfo) :
    m_renderer(imagePoolCreateInfo.renderer) {

}

ImagePool::ResourceHandle ImagePool::create(const duk::rhi::ImageDataSource* imageDataSource) {
    duk::rhi::RHI::ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.commandQueue = m_renderer->main_command_queue();
    imageCreateInfo.initialLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
    imageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::STATIC;
    imageCreateInfo.usage = duk::rhi::Image::Usage::SAMPLED;
    imageCreateInfo.imageDataSource = imageDataSource;

    auto result = m_renderer->rhi()->create_image(imageCreateInfo);

    if (!result) {
        throw std::runtime_error("failed to create image for image pool");
    }

    return insert(result.value());
}

ImagePool::ResourceHandle ImagePool::load(const std::string& path, duk::rhi::PixelFormat format) {
    const auto imageDataSource = duk::import::Importer::instance()->load_image(path, format);
    return create(imageDataSource.get());
}


}
