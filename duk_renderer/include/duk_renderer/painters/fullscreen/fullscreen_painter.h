/// 05/10/2023
/// fullscreen_painter.h

#ifndef DUK_RENDERER_FULLSCREEN_PAINTER_H
#define DUK_RENDERER_FULLSCREEN_PAINTER_H

#include <duk_renderer/painters/painter.h>
#include <duk_renderer/painters/fullscreen/fullscreen_shader_data_source.h>

namespace duk::renderer {

class FullscreenPalette;

struct FullscreenPainterCreateInfo {
    rhi::RHI* rhi;
    rhi::CommandQueue* commandQueue;
};

class FullscreenPainter : public Painter {
public:

    explicit FullscreenPainter(const FullscreenPainterCreateInfo& fullscreenPainterCreateInfo);

    ~FullscreenPainter() override;

    const duk::rhi::DescriptorSetDescription& descriptor_set_description() const;

    std::shared_ptr<FullscreenPalette> make_palette() const;

private:
    rhi::CommandQueue* m_commandQueue;
    FullscreenShaderDataSource m_shaderDataSource;
};

}

#endif // DUK_RENDERER_FULLSCREEN_PAINTER_H

