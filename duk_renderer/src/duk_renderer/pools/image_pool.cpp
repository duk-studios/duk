//
// Created by rov on 11/19/2023.
//

#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/image_data_source.h>

namespace duk::renderer {

ImagePool::ImagePool(const ImagePoolCreateInfo& imagePoolCreateInfo) :
    m_renderer(imagePoolCreateInfo.renderer) {
    {
        duk::rhi::ImageDataSourceRGBA8U whiteImageDataSource(1, 1);
        whiteImageDataSource.at(0, 0) = {255, 255, 255, 255};
        whiteImageDataSource.update_hash();
        m_whiteImage = create(duk::resource::Id(1), &whiteImageDataSource);
    }
    {
        duk::rhi::ImageDataSourceRGBA8U blackImageDataSource(1, 1);
        blackImageDataSource.at(0, 0) = {0, 0, 0, 255};
        blackImageDataSource.update_hash();
        m_blackImage = create(duk::resource::Id(2), &blackImageDataSource);
    }
}

ImageResource ImagePool::create(duk::resource::Id resourceId, const duk::rhi::ImageDataSource* imageDataSource) {
    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.imageDataSource = imageDataSource;
    imageCreateInfo.renderer = m_renderer;

    return insert(resourceId, std::make_shared<Image>(imageCreateInfo));
}

ImageResource ImagePool::white_image() const {
    return m_whiteImage;
}

ImageResource ImagePool::black_image() const {
    return m_blackImage;
}

}
