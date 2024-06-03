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
    MaterialData materialData;
};

class Material {
public:
    explicit Material(const MaterialCreateInfo& materialCreateInfo);

    ~Material();

    DUK_NO_DISCARD ShaderPipelineResource shader() const;

    DUK_NO_DISCARD uint32_t binding_count() const;

    uint32_t find_binding(const std::string_view& name) const;

    uint32_t find_member(uint32_t binding, const std::string_view& name) const;

    void push(const duk::objects::Id& id);

    void clear();

    // returns the current bound descriptor for the given binding
    duk::rhi::Descriptor get(const MaterialLocationId& binding) const;

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

    template<typename Solver>
    void solve_resources(Solver* solver);

private:
    // unfortunately this has to be here until we refactor the way we load resources
    // see DUK-79 for more details
    void init();

private:
    Renderer* m_renderer;
    ShaderPipelineResource m_shader;
    std::vector<Binding> m_bindings;
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
    std::unordered_map<uint32_t, std::unique_ptr<MaterialUniformBuffer>> m_uniformBuffers;
    std::unordered_map<uint32_t, std::unique_ptr<MaterialInstanceBuffer>> m_instanceBuffers;
    bool m_dirty;
};

std::shared_ptr<Material> create_color_material(Renderer* renderer, bool transparent);

std::shared_ptr<Material> create_phong_material(Renderer* renderer);

std::shared_ptr<Material> create_text_material(Renderer* renderer);

using MaterialResource = duk::resource::Handle<Material>;

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

template<typename Solver>
void Material::solve_resources(Solver* solver) {
    solver->solve(m_shader);
    for (auto& binding: m_bindings) {
        switch (binding.type) {
            case BindingType::IMAGE_SAMPLER: {
                auto imageSampler = static_cast<ImageSamplerBinding*>(binding.data.get());
                solver->solve(imageSampler->image);
                break;
            }
            case BindingType::IMAGE: {
                auto image = static_cast<ImageBinding*>(binding.data.get());
                solver->solve(image->image);
                break;
            }
            default:
                break;
        }
    }
}

}// namespace duk::renderer

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::Material& material) {
    material.solve_resources(solver);
}

}// namespace duk::resource

#endif// DUK_RENDERER_MATERIAL_H
