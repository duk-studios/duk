/// 12/09/2023
/// color_material.h

#ifndef DUK_RENDERER_COLOR_MATERIAL_H
#define DUK_RENDERER_COLOR_MATERIAL_H

#include <duk_renderer/resources/materials/material.h>
#include <duk_renderer/resources/materials/color/color_types.h>
#include <duk_renderer/resources/materials/color/color_descriptor_sets.h>
#include <duk_renderer/pools/image_pool.h>

#include <duk_scene/limits.h>
#include <duk_rhi/rhi.h>
#include <duk_rhi/descriptor_set.h>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


namespace duk::renderer {

class ColorMaterialDataSource : public MaterialDataSource {
public:
    ColorMaterialDataSource();

public:
    glm::vec4 color;
    ImageResource baseColorImage;
    duk::rhi::Sampler baseColorSampler;

protected:
    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;
};

struct ColorMaterialCreateInfo {
    Renderer* renderer;
    const ColorMaterialDataSource* colorMaterialDataSource;
};

class ColorMaterial : public Material {
public:

    explicit ColorMaterial(const ColorMaterialCreateInfo& colorMaterialCreateInfo);

    void set_color(const glm::vec4& color);

    void flush_instances() override;

    void insert_instance(const InsertInstanceParams& params) override;

    void clear_instances() override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

private:
    std::unique_ptr<color::InstanceSBO> m_instanceSBO;
    std::unique_ptr<color::MaterialUBO> m_materialUBO;
    ColorDescriptorSet m_descriptorSet;
};

using ColorMaterialResource = duk::pool::Resource<ColorMaterial>;

}

#endif // DUK_RENDERER_COLOR_MATERIAL_H

