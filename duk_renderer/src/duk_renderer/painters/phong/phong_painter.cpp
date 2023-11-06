/// 12/10/2023
/// phong_painter.cpp

#include <duk_renderer/painters/phong/phong_painter.h>
#include <duk_renderer/painters/phong/phong_palette.h>

namespace duk::renderer {


PhongPainter::PhongPainter(const PhongPainterCreateInfo& phongPainterCreateInfo) :
    Painter(phongPainterCreateInfo.rhi),
    m_commandQueue(phongPainterCreateInfo.commandQueue) {
    init_shader(&m_shaderDataSource);
}

PhongPainter::~PhongPainter() = default;

const duk::rhi::DescriptorSetDescription& PhongPainter::descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

std::shared_ptr<PhongPalette> PhongPainter::make_palette() const {
    PhongPaletteCreateInfo colorPaletteCreateInfo = {};
    colorPaletteCreateInfo.rhi = m_rhi;
    colorPaletteCreateInfo.commandQueue = m_commandQueue;
    colorPaletteCreateInfo.painter = this;
    return std::make_shared<PhongPalette>(colorPaletteCreateInfo);
}

}