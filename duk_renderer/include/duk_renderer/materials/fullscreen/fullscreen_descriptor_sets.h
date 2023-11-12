#ifndef DUK_RENDERER_FULLSCREEN_DESCRIPTOR_SETS_H
#define DUK_RENDERER_FULLSCREEN_DESCRIPTOR_SETS_H

#include <duk_renderer/materials/fullscreen/fullscreen_shader_data_source.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

struct FullscreenDescriptorSetCreateInfo {
    duk::rhi::RHI* rhi;
    const FullscreenShaderDataSource* shaderDataSource;
};

class FullscreenDescriptorSet {
public:

    enum class Bindings : uint32_t {
        uTexture = 0
    };

public:

    FullscreenDescriptorSet(const FullscreenDescriptorSetCreateInfo& descriptorSetCreateInfo);

    void set(Bindings binding, const duk::rhi::Descriptor& descriptor);

    duk::rhi::Descriptor& at(Bindings binding);

    duk::rhi::Descriptor& at(uint32_t bindingIndex);

    duk::rhi::DescriptorSet* handle();

    void flush();

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
};

} // namespace duk::renderer

#endif // DUK_RENDERER_FULLSCREEN_DESCRIPTOR_SETS_H
