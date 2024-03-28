/// 12/09/2023
/// material.h

#ifndef DUK_RENDERER_MATERIAL_H
#define DUK_RENDERER_MATERIAL_H

#include <duk_log/log.h>
#include <duk_renderer/material/draw_entry.h>
#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_renderer/material/pipeline.h>

namespace duk::renderer {

struct MaterialDescriptorSetCreateInfo {
    Renderer* renderer;
};

class MaterialDataSource : public duk::hash::DataSource {
public:
    DUK_NO_DISCARD virtual const duk::rhi::ShaderDataSource* shader_data_source() const = 0;

    DUK_NO_DISCARD virtual std::unique_ptr<MaterialDescriptorSet> create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const = 0;
};

struct MaterialCreateInfo {
    Renderer* renderer;
    const MaterialDataSource* materialDataSource;
};

class Material {
public:
    explicit Material(const MaterialCreateInfo& materialCreateInfo);

    ~Material();

    DUK_NO_DISCARD Pipeline* pipeline();

    DUK_NO_DISCARD const Pipeline* pipeline() const;

    DUK_NO_DISCARD MaterialDescriptorSet* descriptor_set();

    DUK_NO_DISCARD InstanceBuffer* instance_buffer();

    void update(const DrawParams& drawParams);

    void bind(duk::rhi::CommandBuffer* commandBuffer);

private:
    Pipeline m_pipeline;
    std::unique_ptr<MaterialDescriptorSet> m_descriptorSet;
};

using MaterialResource = duk::resource::ResourceT<Material>;

}// namespace duk::renderer

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::Material& material) {
    for (auto& image: material.descriptor_set()->images()) {
        solver->solve(image);
    }
}

}// namespace duk::resource

#endif// DUK_RENDERER_MATERIAL_H
