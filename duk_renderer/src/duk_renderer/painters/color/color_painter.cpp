/// 20/08/2023
/// color_shader.cpp

#include <duk_renderer/painters/color/color_painter.h>
#include <duk_renderer/painters/color/color_palette.h>

namespace duk::renderer {

ColorPainter::ColorPainter(const ColorPainterCreateInfo& colorPainterCreateInfo) :
    Painter(colorPainterCreateInfo.rhi),
    m_commandQueue(colorPainterCreateInfo.commandQueue) {
    init_shader(&m_shaderDataSource);
}

const duk::rhi::DescriptorSetDescription& ColorPainter::descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

std::shared_ptr<ColorPalette> ColorPainter::make_palette() const {
    ColorPaletteCreateInfo colorPaletteCreateInfo = {};
    colorPaletteCreateInfo.rhi = m_rhi;
    colorPaletteCreateInfo.commandQueue = m_commandQueue;
    colorPaletteCreateInfo.painter = this;
    return std::make_shared<ColorPalette>(colorPaletteCreateInfo);
}

}