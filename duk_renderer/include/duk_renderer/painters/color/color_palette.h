/// 12/09/2023
/// color_palette.h

#ifndef DUK_RENDERER_COLOR_PALETTE_H
#define DUK_RENDERER_COLOR_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_renderer/painters/color/color_painter.h>
#include <duk_rhi/rhi.h>
#include <duk_rhi/descriptor_set.h>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct ColorPaletteCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    const ColorPainter* painter;
};

class ColorPalette : public Palette {
public:

    explicit ColorPalette(const ColorPaletteCreateInfo& colorPaletteCreateInfo);

    void set_color(const glm::vec4& color);

    void update(const UpdateParams& params) override;

    void apply(duk::rhi::CommandBuffer* commandBuffer) override;

private:
    ColorPainter::Transform m_transformData;
    ColorPainter::Material m_materialData;
    std::shared_ptr<duk::rhi::Buffer> m_transformUBO;
    std::shared_ptr<duk::rhi::Buffer> m_materialUBO;
    std::shared_ptr<duk::rhi::DescriptorSet> m_instanceDescriptorSet;

};

}

#endif // DUK_RENDERER_COLOR_PALETTE_H

