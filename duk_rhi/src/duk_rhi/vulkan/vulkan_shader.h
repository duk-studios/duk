/// 17/05/2023
/// vulkan_shader.h

#ifndef DUK_RHI_VULKAN_SHADER_H
#define DUK_RHI_VULKAN_SHADER_H

#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/pipeline/shader_data_source.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

VkShaderStageFlagBits convert_module(ShaderModule::Bits module);

VkShaderStageFlags convert_module_mask(ShaderModule::Mask moduleMask);

struct VulkanShaderCreateInfo {
    VkDevice device;
    const ShaderDataSource* shaderDataSource;
    VulkanDescriptorSetLayoutCache* descriptorSetLayoutCache;
};

class VulkanShader : public Shader {
public:
    explicit VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo);

    ~VulkanShader() override;

    DUK_NO_DISCARD const std::vector<VkVertexInputAttributeDescription>& input_attributes() const;

    DUK_NO_DISCARD const std::vector<VkVertexInputBindingDescription>& input_bindings() const;

    DUK_NO_DISCARD const std::unordered_map<ShaderModule::Bits, VkShaderModule>& shader_modules() const;

    DUK_NO_DISCARD const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts() const;

    DUK_NO_DISCARD const VkPipelineLayout& pipeline_layout() const;

    DUK_NO_DISCARD const DescriptorSetDescription& descriptor_set_description(uint32_t set) const override;

    DUK_NO_DISCARD const VertexLayout& vertex_layout() const override;

    DUK_NO_DISCARD bool is_graphics_shader() const override;

    DUK_NO_DISCARD bool is_compute_shader() const override;

    DUK_NO_DISCARD hash::Hash hash() const override;

private:
    bool create_shader_module(ShaderModule::Bits type, const ShaderDataSource* shaderDataSource);

private:
    VkDevice m_device;
    duk::hash::Hash m_hash;
    ShaderModule::Mask m_moduleMask;
    std::vector<DescriptorSetDescription> m_descriptorSetDescriptions;
    VertexLayout m_vertexLayout;
    std::unordered_map<ShaderModule::Bits, VkShaderModule> m_shaderModules;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    VkPipelineLayout m_pipelineLayout;
    std::vector<VkVertexInputBindingDescription> m_inputBindings;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_SHADER_H
