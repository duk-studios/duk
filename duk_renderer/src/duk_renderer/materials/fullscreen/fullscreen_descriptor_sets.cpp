#include <duk_renderer/materials/fullscreen/fullscreen_descriptor_sets.h>

namespace duk::renderer {

FullscreenDescriptorSet::FullscreenDescriptorSet(const FullscreenDescriptorSetCreateInfo& createInfo) {

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = createInfo.shaderDataSource->descriptor_set_descriptions().at(0);

    auto result = createInfo.rhi->create_descriptor_set(descriptorSetCreateInfo);

    if (!result) {
        throw std::runtime_error("failed to create FullscreenDescriptorSet: " + result.error().description());
    }

    m_descriptorSet = std::move(result.value());
}

void FullscreenDescriptorSet::set(FullscreenDescriptorSet::Bindings binding, const duk::rhi::Descriptor& descriptor) {
    m_descriptorSet->set(static_cast<uint32_t>(binding), descriptor);
}

duk::rhi::Descriptor& FullscreenDescriptorSet::at(FullscreenDescriptorSet::Bindings binding) {
    return m_descriptorSet->at(static_cast<uint32_t>(binding));
}

duk::rhi::Descriptor& FullscreenDescriptorSet::at(uint32_t bindingIndex) {
    return m_descriptorSet->at(bindingIndex);
}

duk::rhi::DescriptorSet* FullscreenDescriptorSet::handle() {
    return m_descriptorSet.get();
}

void FullscreenDescriptorSet::flush() {
    m_descriptorSet->flush();
}

} // namespace duk::renderer
