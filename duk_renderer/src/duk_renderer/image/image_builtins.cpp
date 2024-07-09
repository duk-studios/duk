//
// Created by rov on 11/19/2023.
//

#include <duk_renderer/image/image_builtins.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/image_data_source.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<Image> create_image(duk::rhi::RHI* rhi, duk::rhi::CommandQueue* commandQueue, const duk::rhi::ImageDataSource* imageDataSource) {
    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.imageDataSource = imageDataSource;
    imageCreateInfo.rhi = rhi;
    imageCreateInfo.commandQueue = commandQueue;
    return std::make_shared<Image>(imageCreateInfo);
}

}// namespace detail

ImageBuiltins::ImageBuiltins(const ImageBuiltinsCreateInfo& imageBuiltinsCreateInfo) {
    const auto rhi = imageBuiltinsCreateInfo.rhi;
    const auto commandQueue = imageBuiltinsCreateInfo.commandQueue;
    const auto pools = imageBuiltinsCreateInfo.pools;
    {
        duk::rhi::ImageDataSourceRGBA8U whiteImageDataSource(1, 1);
        whiteImageDataSource.at(0, 0) = {255, 255, 255, 255};
        whiteImageDataSource.update_hash();
        m_white = pools->insert(kWhiteImageId, detail::create_image(rhi, commandQueue, &whiteImageDataSource));
    }
    {
        duk::rhi::ImageDataSourceRGBA8U blackImageDataSource(1, 1);
        blackImageDataSource.at(0, 0) = {0, 0, 0, 255};
        blackImageDataSource.update_hash();
        m_black = pools->insert(kBlackImageId, detail::create_image(rhi, commandQueue, &blackImageDataSource));
    }
}

duk::resource::Handle<Image> ImageBuiltins::white() const {
    return m_white;
}

duk::resource::Handle<Image> ImageBuiltins::black() const {
    return m_black;
}

}// namespace duk::renderer
