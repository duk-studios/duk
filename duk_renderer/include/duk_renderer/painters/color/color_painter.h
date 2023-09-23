/// 20/08/2023
/// color_shader.h

#ifndef DUK_RENDERER_COLOR_SHADER_H
#define DUK_RENDERER_COLOR_SHADER_H

#include <duk_renderer/painters/painter.h>

#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/pipeline/std_shader_data_source.h>
#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/rhi.h>

#include <glm/matrix.hpp>


namespace duk::renderer {

class ColorPalette;

struct ColorPainterCreateInfo {
    rhi::RHI* rhi;
    rhi::CommandQueue* commandQueue;
};

class ColorPainter : public Painter {
public:

    explicit ColorPainter(const ColorPainterCreateInfo& colorPainterCreateInfo);

    DUK_NO_DISCARD const duk::rhi::ShaderDataSource* shader_data_source() const override;

    DUK_NO_DISCARD const duk::rhi::DescriptorSetDescription& global_descriptor_set_description() const;

    DUK_NO_DISCARD const duk::rhi::DescriptorSetDescription& instance_descriptor_set_description() const;

    std::shared_ptr<ColorPalette> make_palette() const;

private:
    rhi::CommandQueue* m_commandQueue;
    duk::rhi::StdShaderDataSource m_shaderDataSource;
};

}

#endif // DUK_RENDERER_COLOR_SHADER_H
