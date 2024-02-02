#ifndef DUK_RENDERER_COLOR_DESCRIPTOR_SETS_H
#define DUK_RENDERER_COLOR_DESCRIPTOR_SETS_H

#include <duk_renderer/resources/materials/color/color_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct ColorDescriptorSetCreateInfo {
    duk::rhi::RHI* rhi;
    const ColorShaderDataSource* shaderDataSource;
};

class ColorDescriptorSet {
public:

    enum class Bindings : uint32_t {
        uCamera = 0,
        uInstances = 1,
        uMaterial = 2,
        uBaseColor = 3
    };

public:

    ColorDescriptorSet(const ColorDescriptorSetCreateInfo& descriptorSetCreateInfo);

    void set(Bindings binding, const duk::rhi::Descriptor& descriptor);

    duk::rhi::Descriptor& at(Bindings binding);

    duk::rhi::Descriptor& at(uint32_t bindingIndex);

    duk::rhi::DescriptorSet* handle();

    void flush();

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
};

} // namespace duk::renderer

#endif // DUK_RENDERER_COLOR_DESCRIPTOR_SETS_H
