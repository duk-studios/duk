/// 05/10/2023
/// fullscreen_painter.cpp

#include <duk_renderer/painters/fullscreen/fullscreen_painter.h>
#include <duk_renderer/painters/fullscreen/fullscreen_palette.h>

namespace duk::renderer {

FullscreenPainter::FullscreenPainter(const FullscreenPainterCreateInfo& fullscreenPainterCreateInfo) :
    Painter(fullscreenPainterCreateInfo.rhi),
    m_commandQueue(fullscreenPainterCreateInfo.commandQueue) {
    init_shader(&m_shaderDataSource);
}

FullscreenPainter::~FullscreenPainter() = default;

const duk::rhi::DescriptorSetDescription& FullscreenPainter::descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

std::shared_ptr<FullscreenPalette> FullscreenPainter::make_palette() const {
    FullscreenPaletteCreateInfo fullscreenPaletteCreateInfo = {};
    fullscreenPaletteCreateInfo.rhi = m_rhi;
    fullscreenPaletteCreateInfo.commandQueue = m_commandQueue;
    fullscreenPaletteCreateInfo.painter = this;
    return std::make_shared<FullscreenPalette>(fullscreenPaletteCreateInfo);
}

}