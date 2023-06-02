/// 17/05/2023
/// vulkan_shader.h

#ifndef DUK_RENDERER_VULKAN_SHADER_H
#define DUK_RENDERER_VULKAN_SHADER_H

#include <duk_renderer/pipeline/shader.h>
#include <duk_renderer/pipeline/shader_data_source.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_descriptor_set.h>

namespace duk::renderer {

namespace vk {

VkShaderStageFlagBits convert_module(Shader::Module::Bits module);

VkShaderStageFlags convert_module_mask(Shader::Module::Mask moduleMask);

}

struct VulkanShaderCreateInfo {
    VkDevice device;
    ShaderDataSource* shaderDataSource;
    VulkanDescriptorSetLayoutCache* descriptorSetLayoutCache;
};

class VulkanShader : public Shader {
public:

    explicit VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo);

    ~VulkanShader() override;

    DUK_NO_DISCARD const std::vector<VkVertexInputAttributeDescription>& input_attributes() const;

    DUK_NO_DISCARD const std::vector<VkVertexInputBindingDescription>& input_bindings() const;

    DUK_NO_DISCARD const std::unordered_map<Shader::Module::Bits, VkShaderModule>& shader_modules() const;

    DUK_NO_DISCARD const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts() const;

    DUK_NO_DISCARD const VkPipelineLayout& pipeline_layout() const;

    DUK_NO_DISCARD hash::Hash hash() const override;

    DUK_NO_DISCARD bool is_graphics_shader() const override;

    DUK_NO_DISCARD bool is_compute_shader() const override;

private:

    bool create_shader_module(Shader::Module::Bits type, ShaderDataSource* shaderDataSource);

private:
    VkDevice m_device;
    duk::hash::Hash m_hash;
    Shader::Module::Mask m_moduleMask;
    std::unordered_map<Shader::Module::Bits, VkShaderModule> m_shaderModules;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    VkPipelineLayout m_pipelineLayout;
    std::vector<VkVertexInputBindingDescription> m_inputBindings;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;

};

}

#endif // DUK_RENDERER_VULKAN_SHADER_H

