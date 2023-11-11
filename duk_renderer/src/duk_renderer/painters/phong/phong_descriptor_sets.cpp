#include <duk_renderer/painters/phong/phong_descriptor_sets.h>

namespace duk::renderer {

PhongDescriptorSet::PhongDescriptorSet(const PhongDescriptorSetCreateInfo& createInfo) {

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = createInfo.shaderDataSource->descriptor_set_descriptions().at(0);

    auto result = createInfo.rhi->create_descriptor_set(descriptorSetCreateInfo);

    if (!result) {
        throw std::runtime_error("failed to create PhongDescriptorSet: " + result.error().description());
    }

    m_descriptorSet = std::move(result.value());
}

void PhongDescriptorSet::set(PhongDescriptorSet::Bindings binding, const duk::rhi::Descriptor& descriptor) {
    m_descriptorSet->set(static_cast<uint32_t>(binding), descriptor);
}

duk::rhi::Descriptor& PhongDescriptorSet::at(PhongDescriptorSet::Bindings binding) {
    return m_descriptorSet->at(static_cast<uint32_t>(binding));
}

duk::rhi::Descriptor& PhongDescriptorSet::at(uint32_t bindingIndex) {
    return m_descriptorSet->at(bindingIndex);
}

duk::rhi::DescriptorSet* PhongDescriptorSet::handle() {
    return m_descriptorSet.get();
}

void PhongDescriptorSet::flush() {
    m_descriptorSet->flush();
}

} // namespace duk::renderer
