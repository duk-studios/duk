/// 13/10/2023
/// phong_palette.h

#ifndef DUK_RENDERER_PHONG_PALETTE_H
#define DUK_RENDERER_PHONG_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_renderer/painters/phong/phong_types.h>

#include <duk_rhi/rhi.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

class PhongPainter;

struct PhongPaletteCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    const PhongPainter* painter;
};

class PhongPalette : public Palette {
public:

    explicit PhongPalette(const PhongPaletteCreateInfo& phongPaletteCreateInfo);

    void clear_instances() override;

    void insert_instance(const InsertInstanceParams& params) override;

    void flush_instances() override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<phong::TransformSBO> m_transformSBO;
};

}

#endif // DUK_RENDERER_PHONG_PALETTE_H

