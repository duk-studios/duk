/// 13/10/2023
/// phong_material.h

#ifndef DUK_RENDERER_PHONG_MATERIAL_H
#define DUK_RENDERER_PHONG_MATERIAL_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/materials/material.h>
#include <duk_renderer/materials/phong/phong_types.h>
#include <duk_renderer/materials/phong/phong_shader_data_source.h>
#include <duk_renderer/materials/phong/phong_descriptor_sets.h>

#include <duk_rhi/rhi.h>

#include <glm/mat4x4.hpp>

namespace duk::renderer {

struct PhongMaterialCreateInfo {
    Renderer* renderer;
    const PhongShaderDataSource* shaderDataSource;
};

class PhongMaterial : public Material {
public:

    explicit PhongMaterial(const PhongMaterialCreateInfo& phongMaterialCreateInfo);

    void clear_instances() override;

    void insert_instance(const InsertInstanceParams& params) override;

    void flush_instances() override;

    void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) override;

    void update_base_color_image(duk::rhi::Image* image, const duk::rhi::Sampler& sampler);

    void update_base_color(const glm::vec3& color);

    void update_shininess_image(duk::rhi::Image* image, const duk::rhi::Sampler& sampler);

    void update_shininess(float shininess);

private:
    PhongDescriptorSet m_descriptorSet;
    std::unique_ptr<phong::TransformSBO> m_transformSBO;
    std::unique_ptr<phong::MaterialUBO> m_materialUBO;
};

}

#endif // DUK_RENDERER_PHONG_MATERIAL_H

