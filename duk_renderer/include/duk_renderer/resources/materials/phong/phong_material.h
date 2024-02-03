/// 13/10/2023
/// phong_material.h

#ifndef DUK_RENDERER_PHONG_MATERIAL_H
#define DUK_RENDERER_PHONG_MATERIAL_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/resources/materials/mesh_material.h>
#include <duk_renderer/resources/materials/phong/phong_types.h>
#include <duk_renderer/resources/materials/phong/phong_shader_data_source.h>
#include <duk_renderer/resources/materials/phong/phong_descriptor_sets.h>

#include <duk_rhi/rhi.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

class PhongMaterialDataSource : public MaterialDataSource {
public:
    PhongMaterialDataSource();

public:
    glm::vec4 baseColor;
    ImageResource baseColorImage;
    duk::rhi::Sampler baseColorSampler;
    float shininess;
    ImageResource shininessImage;
    duk::rhi::Sampler shininessSampler;

protected:
    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override;
};

struct PhongMaterialCreateInfo {
    Renderer* renderer;
    const PhongMaterialDataSource* phongMaterialDataSource;
};

class PhongMaterial : public MeshMaterial {
public:

    explicit PhongMaterial(const PhongMaterialCreateInfo& phongMaterialCreateInfo);

    void clear_instances() override;

    void insert_instance(const InsertInstanceParams& params) override;

    void flush_instances() override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) override;

    void update_base_color_image(const ImageResource& baseColorImage, const duk::rhi::Sampler& sampler);

    void update_base_color(const glm::vec3& color);

    void update_shininess_image(const ImageResource& shininessImage, const duk::rhi::Sampler& sampler);

    void update_shininess(float shininess);

private:
    PhongDescriptorSet m_descriptorSet;
    std::unique_ptr<phong::TransformSBO> m_transformSBO;
    std::unique_ptr<phong::MaterialUBO> m_materialUBO;
    ImageResource m_baseColorImage;
    duk::rhi::Sampler m_baseColorSampler;
    ImageResource m_shininessImage;
    duk::rhi::Sampler m_shininessSampler;
};

using PhongMaterialResource = duk::resource::ResourceT<PhongMaterial>;

}

#endif // DUK_RENDERER_PHONG_MATERIAL_H

