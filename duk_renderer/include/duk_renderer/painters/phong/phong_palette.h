/// 13/10/2023
/// phong_palette.h

#ifndef DUK_RENDERER_PHONG_PALETTE_H
#define DUK_RENDERER_PHONG_PALETTE_H

#include <duk_renderer/painters/palette.h>
#include <duk_rhi/rhi.h>
#include <glm/mat4x4.hpp>
#include <duk_renderer/painters/camera.h>
#include <duk_renderer/painters/storage_buffer.h>

namespace duk::renderer {

class PhongPainter;

struct PhongPaletteCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    const PhongPainter* painter;
};

class PhongPalette : public Palette {
public:

    struct Transform {
        glm::mat4 model;
    };

    struct LightValue {
        glm::vec3 color;
        float intensity;
    };

    struct DirectionalLight {
        LightValue value;
        glm::vec3 direction;
    };

public:

    explicit PhongPalette(const PhongPaletteCreateInfo& phongPaletteCreateInfo);

    void insert_instance(const InsertInstanceParams& params) override;

    void apply(duk::rhi::CommandBuffer* commandBuffer) override;

    void clear() override;

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unique_ptr<CameraUBO> m_cameraUBO;
    std::unique_ptr<StorageBuffer<Transform>> m_transformSBO;
    std::unique_ptr<UniformBuffer<DirectionalLight>> m_lightUBO;
};

}

#endif // DUK_RENDERER_PHONG_PALETTE_H

