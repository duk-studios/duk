/// 17/05/2023
/// vulkan_shader.h

#ifndef DUK_RHI_VULKAN_SHADER_H
#define DUK_RHI_VULKAN_SHADER_H

#include <duk_rhi/shader.h>
#include <duk_rhi/binding_layout.h>
#include <duk_rhi/vertex_layout.h>
#include <duk_rhi/shader_data_source.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

VkShaderStageFlagBits convert_module(ShaderModule::Bits module);

VkShaderStageFlags convert_module_mask(ShaderModule::Mask moduleMask);

struct VulkanShaderCreateInfo {
    VkDevice device;
    const ShaderDataSource* shaderDataSource;
};

/// Stores the resolved Vulkan (set, binding) for a logical descriptor slot.
struct BindingLocation {
    uint32_t set;
    uint32_t binding;
};

class VulkanShader : public Shader {
public:
    explicit VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo);

    ~VulkanShader() override;

    DUK_NO_DISCARD const std::vector<VkVertexInputAttributeDescription>& input_attributes() const;

    DUK_NO_DISCARD const std::vector<VkVertexInputBindingDescription>& input_bindings() const;

    DUK_NO_DISCARD const std::unordered_map<ShaderModule::Bits, VkShaderModule>& shader_modules() const;

    /// Returns the Vulkan (set, binding) for the given logical binding slot.
    DUK_NO_DISCARD const BindingLocation& binding_location(uint32_t logicalIndex) const;

    // Shader interface
    DUK_NO_DISCARD const BindingLayout& binding_layout() const override;
    DUK_NO_DISCARD const VertexLayout& vertex_layout() const override;
    DUK_NO_DISCARD bool is_graphics_shader() const override;
    DUK_NO_DISCARD bool is_compute_shader() const override;
    DUK_NO_DISCARD hash::Hash hash() const override;

private:
    VkDevice m_device;
    duk::hash::Hash m_hash;
    ShaderModule::Mask m_moduleMask;

    /// Flat backend-agnostic binding layout sourced from the ShaderDataSource.
    BindingLayout m_bindingLayout;

    /// Maps logical binding index → Vulkan (set, binding).
    std::vector<BindingLocation> m_bindingRemapTable;

    VertexLayout m_vertexLayout;
    std::unordered_map<ShaderModule::Bits, VkShaderModule> m_shaderModules;
    std::vector<VkVertexInputBindingDescription> m_inputBindings;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_SHADER_H
