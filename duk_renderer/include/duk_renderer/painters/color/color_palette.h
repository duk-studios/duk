/// 12/09/2023
/// color_palette.h

#ifndef DUK_RENDERER_COLOR_PALETTE_H
#define DUK_RENDERER_COLOR_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_renderer/painters/color/color_painter.h>
#include <duk_renderer/painters/uniform_buffer.h>
#include <duk_renderer/painters/storage_buffer.h>
#include <duk_renderer/painters/camera.h>

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

    struct Transform {
        glm::mat4 model;
    };

    struct Material {
        glm::vec4 color;
    };

public:

    explicit ColorPalette(const ColorPaletteCreateInfo& colorPaletteCreateInfo);

    void set_color(const glm::vec4& color);

    void insert_instance(const InsertInstanceParams& params) override;

    void apply(duk::rhi::CommandBuffer* commandBuffer) override;

    void clear() override;

private:
    std::unique_ptr<CameraUBO> m_cameraUBO;
    std::unique_ptr<StorageBuffer<Transform>> m_transformSBO;
    std::unique_ptr<UniformBuffer<Material>> m_materialUBO;
    std::shared_ptr<duk::rhi::DescriptorSet> m_globalDescriptorSet;
    std::shared_ptr<duk::rhi::DescriptorSet> m_instanceDescriptorSet;

};

}

#endif // DUK_RENDERER_COLOR_PALETTE_H

