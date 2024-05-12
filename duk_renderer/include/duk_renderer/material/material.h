/// 12/09/2023
/// material.h

#ifndef DUK_RENDERER_MATERIAL_H
#define DUK_RENDERER_MATERIAL_H

#include <duk_renderer/texture.h>
#include <duk_renderer/image/image.h>
#include <duk_renderer/material/material_data.h>
#include <duk_renderer/material/material_buffer.h>
#include <duk_renderer/material/instance_buffer.h>
#include <duk_renderer/shader/shader_pipeline.h>

namespace duk::renderer {

static constexpr uint32_t kInvalidMaterialLocation = ~0;

class MaterialLocationId {
public:
    MaterialLocationId();

    MaterialLocationId(uint32_t index);

    MaterialLocationId(std::string name);

    MaterialLocationId(const char* name);

    uint32_t index() const;

    std::string_view name() const;

    bool valid() const;

private:
    uint32_t m_index;
    std::string m_name;
};

struct MaterialCreateInfo {
    Renderer* renderer;
    const MaterialData* materialData;
};

class Material {
public:

    explicit Material(const MaterialCreateInfo& materialCreateInfo);

    ~Material();

    DUK_NO_DISCARD uint32_t binding_count() const;

    DUK_NO_DISCARD std::unordered_map<uint32_t, ImageResource> images();

    DUK_NO_DISCARD std::unordered_map<uint32_t, ImageSamplerBinding> image_samplers();

    uint32_t find_binding(const std::string_view& name) const;

    uint32_t find_member(uint32_t binding, const std::string_view& name) const;

    void push(const duk::objects::Id& id);

    void clear();

    void set(const MaterialLocationId& binding, const ImageResource& image);

    void set(const MaterialLocationId& binding, const ImageResource& image, const duk::rhi::Sampler& sampler);

    void set(const MaterialLocationId& binding, const duk::rhi::Descriptor& descriptor);

    // sets the content of an entire buffer
    void set(const MaterialLocationId& binding, const void* data, uint32_t size);

    // typed override for the method above
    template<typename T>
    void set(const MaterialLocationId& binding, const T& data);

    // sets the content of a specific member of a buffer
    void set(const MaterialLocationId& binding, const MaterialLocationId& member, const void* data, uint32_t size);

    // typed override for the method above
    template<typename T>
    void set(const MaterialLocationId& binding, const MaterialLocationId& member, const T& data);

    // sets the content of an entire instance in an instance buffer
    void set(const duk::objects::Id& id, const MaterialLocationId& binding, const void* data, uint32_t size);

    // typed override for the method above
    template<typename T>
    void set(const duk::objects::Id& id, const MaterialLocationId& binding, const T& data);

    // sets the content of a specific member of an instance in an instance buffer
    void set(const duk::objects::Id& id, const MaterialLocationId& binding, const MaterialLocationId& member, const void* data, uint32_t size);

    // typed override for the method above
    template<typename T>
    void set(const duk::objects::Id& id, const MaterialLocationId& binding, const MaterialLocationId& member, const T& data);

    void update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport);

    void bind(duk::rhi::CommandBuffer* commandBuffer);

private:
    Renderer* m_renderer;
    ShaderPipelineResource m_shader;
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unordered_map<uint32_t, ImageResource> m_images;
    std::unordered_map<uint32_t, ImageSamplerBinding> m_imageSamplers;
    std::unordered_map<uint32_t, MaterialUniformBuffer> m_uniformBuffers;
    std::unordered_map<uint32_t, MaterialInstanceBuffer> m_instanceBuffers;
    bool m_dirty;
};

Material* find_material(const duk::objects::Object& object);

using MaterialResource = duk::resource::ResourceT<Material>;

template<typename T>
void Material::set(const MaterialLocationId& binding, const T& data) {
    set(binding, &data, sizeof(T));
}

template<typename T>
void Material::set(const MaterialLocationId& binding, const MaterialLocationId& member, const T& data) {
    set(binding, member, &data, sizeof(T));
}

template<typename T>
void Material::set(const duk::objects::Id& id, const MaterialLocationId& binding, const T& data) {
    set(id, binding, &data, sizeof(T));
}

template<typename T>
void Material::set(const duk::objects::Id& id, const MaterialLocationId& binding, const MaterialLocationId& member, const T& data) {
    set(id, binding, member, &data, sizeof(T));
}

}// namespace duk::renderer



namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::Material& material) {
    for (auto& [binding, image]: material.images()) {
        solver->solve(image);
    }
    for (auto& [binding, texture]: material.image_samplers()) {
        solver->solve(texture);
    }
}

}// namespace duk::resource

#endif// DUK_RENDERER_MATERIAL_H
