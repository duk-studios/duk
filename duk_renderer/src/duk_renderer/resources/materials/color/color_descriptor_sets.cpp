#include <duk_renderer/resources/materials/color/color_descriptor_sets.h>

namespace duk::renderer {

ColorDescriptorSet::ColorDescriptorSet(const ColorDescriptorSetCreateInfo& createInfo) {

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = createInfo.shaderDataSource->descriptor_set_descriptions().at(0);

    m_descriptorSet = createInfo.rhi->create_descriptor_set(descriptorSetCreateInfo);
}

void ColorDescriptorSet::set(ColorDescriptorSet::Bindings binding, const duk::rhi::Descriptor& descriptor) {
    m_descriptorSet->set(static_cast<uint32_t>(binding), descriptor);
}

duk::rhi::Descriptor& ColorDescriptorSet::at(ColorDescriptorSet::Bindings binding) {
    return m_descriptorSet->at(static_cast<uint32_t>(binding));
}

duk::rhi::Descriptor& ColorDescriptorSet::at(uint32_t bindingIndex) {
    return m_descriptorSet->at(bindingIndex);
}

duk::rhi::DescriptorSet* ColorDescriptorSet::handle() {
    return m_descriptorSet.get();
}

void ColorDescriptorSet::flush() {
    m_descriptorSet->flush();
}

} // namespace duk::renderer
