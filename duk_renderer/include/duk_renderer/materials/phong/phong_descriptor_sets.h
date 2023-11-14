#ifndef DUK_RENDERER_PHONG_DESCRIPTOR_SETS_H
#define DUK_RENDERER_PHONG_DESCRIPTOR_SETS_H

#include <duk_renderer/materials/phong/phong_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct PhongDescriptorSetCreateInfo {
    duk::rhi::RHI* rhi;
    const PhongShaderDataSource* shaderDataSource;
};

class PhongDescriptorSet {
public:

    enum class Bindings : uint32_t {
        uCamera = 0,
        uTransform = 1,
        uLights = 2,
        uMaterial = 3,
        uBaseColor = 4,
        uSpecular = 5
    };

public:

    PhongDescriptorSet(const PhongDescriptorSetCreateInfo& descriptorSetCreateInfo);

    void set(Bindings binding, const duk::rhi::Descriptor& descriptor);

    duk::rhi::Descriptor& at(Bindings binding);

    duk::rhi::Descriptor& at(uint32_t bindingIndex);

    duk::rhi::DescriptorSet* handle();

    void flush();

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
};

} // namespace duk::renderer

#endif // DUK_RENDERER_PHONG_DESCRIPTOR_SETS_H
