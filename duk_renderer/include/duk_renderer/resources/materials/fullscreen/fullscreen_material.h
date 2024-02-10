/// 05/10/2023
/// fullscreen_material.h

#ifndef DUK_RENDERER_FULLSCREEN_MATERIAL_H
#define DUK_RENDERER_FULLSCREEN_MATERIAL_H

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/resources/materials/fullscreen/fullscreen_descriptor_sets.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct FullscreenMaterialCreateInfo {
    Renderer* renderer;
};

class FullscreenMaterial : public Material {
public:

    explicit FullscreenMaterial(const FullscreenMaterialCreateInfo& fullscreenMaterialCreateInfo);

    void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params);

    void update(duk::rhi::Image* image, const duk::rhi::Sampler& sampler);

private:
    FullscreenDescriptorSet m_descriptorSet;

};

using FullscreenMaterialResource = duk::resource::ResourceT<FullscreenMaterial>;

}

#endif // DUK_RENDERER_FULLSCREEN_MATERIAL_H

