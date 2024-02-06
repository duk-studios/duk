#include <duk_renderer/resources/materials/sprites/sprite_color/sprite_color_descriptor_sets.h>

namespace duk::renderer {

SpriteColorDescriptorSet::SpriteColorDescriptorSet(const SpriteColorDescriptorSetCreateInfo& createInfo) {

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = createInfo.shaderDataSource->descriptor_set_descriptions().at(0);

    auto result = createInfo.rhi->create_descriptor_set(descriptorSetCreateInfo);

    if (!result) {
        throw std::runtime_error("failed to create SpriteColorDescriptorSet!");
    }

    m_descriptorSet = result;
}

void SpriteColorDescriptorSet::set(SpriteColorDescriptorSet::Bindings binding, const duk::rhi::Descriptor& descriptor) {
    m_descriptorSet->set(static_cast<uint32_t>(binding), descriptor);
}

duk::rhi::Descriptor& SpriteColorDescriptorSet::at(SpriteColorDescriptorSet::Bindings binding) {
    return m_descriptorSet->at(static_cast<uint32_t>(binding));
}

duk::rhi::Descriptor& SpriteColorDescriptorSet::at(uint32_t bindingIndex) {
    return m_descriptorSet->at(bindingIndex);
}

duk::rhi::DescriptorSet* SpriteColorDescriptorSet::handle() {
    return m_descriptorSet.get();
}

void SpriteColorDescriptorSet::flush() {
    m_descriptorSet->flush();
}

} // namespace duk::renderer
