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

struct ColorPainterCreateInfo {
    rhi::RHI* rhi;
};

class ColorPainter : public Painter {
public:

    struct Transform {
        glm::mat4 model;
    };

    struct Material {
        glm::vec4 color;
    };

public:

    explicit ColorPainter(const ColorPainterCreateInfo& colorPainterCreateInfo);

    void paint(duk::rhi::CommandBuffer* commandBuffer, const PaintParams& params) override;

    DUK_NO_DISCARD const duk::rhi::ShaderDataSource* shader_data_source() const override;

    DUK_NO_DISCARD const duk::rhi::DescriptorSetDescription& instance_descriptor_set_description() const;

private:
    duk::rhi::StdShaderDataSource m_shaderDataSource;
};

}

#endif // DUK_RENDERER_COLOR_SHADER_H
