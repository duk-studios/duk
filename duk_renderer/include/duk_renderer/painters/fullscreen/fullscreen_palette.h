/// 05/10/2023
/// fullscreen_palette.h

#ifndef DUK_RENDERER_FULLSCREEN_PALETTE_H
#define DUK_RENDERER_FULLSCREEN_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_renderer/painters/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/painters/fullscreen/fullscreen_descriptor_sets.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct FullscreenPaletteCreateInfo {
    Renderer* renderer;
    const FullscreenShaderDataSource* shaderDataSource;
};

class FullscreenPalette : public Palette {
public:

    explicit FullscreenPalette(const FullscreenPaletteCreateInfo& fullscreenPaletteCreateInfo);

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

    void update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler);

private:
    FullscreenDescriptorSet m_descriptorSet;

};

}

#endif // DUK_RENDERER_FULLSCREEN_PALETTE_H

