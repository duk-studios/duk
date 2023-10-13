/// 05/10/2023
/// fullscreen_palette.h

#ifndef DUK_RENDERER_FULLSCREEN_PALETTE_H
#define DUK_RENDERER_FULLSCREEN_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class FullscreenPainter;

struct FullscreenPaletteCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    const FullscreenPainter* painter;
};

class FullscreenPalette : public Palette {
public:

    explicit FullscreenPalette(const FullscreenPaletteCreateInfo& fullscreenPaletteCreateInfo);

    void insert_instance(const InsertInstanceParams& params) override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

    void clear() override;

    void update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler);

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;

};

}

#endif // DUK_RENDERER_FULLSCREEN_PALETTE_H

