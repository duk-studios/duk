/// 12/09/2023
/// color_palette.h

#ifndef DUK_RENDERER_COLOR_PALETTE_H
#define DUK_RENDERER_COLOR_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_renderer/painters/color/color_painter.h>
#include <duk_renderer/painters/color/color_types.h>

#include <duk_scene/limits.h>
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

    void flush_instances() override;

    void insert_instance(const InsertInstanceParams& params) override;

    void clear_instances() override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

private:
    std::unique_ptr<color::InstanceSBO> m_instanceSBO;
    std::unique_ptr<color::MaterialUBO> m_materialUBO;
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;

};

}

#endif // DUK_RENDERER_COLOR_PALETTE_H

