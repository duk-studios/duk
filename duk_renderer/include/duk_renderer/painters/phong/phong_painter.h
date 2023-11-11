/// 12/10/2023
/// phong_painter.h

#ifndef DUK_RENDERER_PHONG_PAINTER_H
#define DUK_RENDERER_PHONG_PAINTER_H

#include <duk_renderer/painters/painter.h>
#include <duk_renderer/painters/phong/phong_shader_data_source.h>

namespace duk::renderer {

class PhongPalette;

struct PhongPainterCreateInfo {
    rhi::RHI* rhi;
    rhi::CommandQueue* commandQueue;
};

class PhongPainter : public Painter {
public:

    explicit PhongPainter(const PhongPainterCreateInfo& phongPainterCreateInfo);

    ~PhongPainter() override;

    std::shared_ptr<PhongPalette> make_palette() const;

private:
    rhi::CommandQueue* m_commandQueue;
    PhongShaderDataSource m_shaderDataSource;
};


}

#endif // DUK_RENDERER_PHONG_PAINTER_H

