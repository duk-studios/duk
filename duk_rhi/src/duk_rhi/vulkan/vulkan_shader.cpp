/// 17/05/2023
/// vulkan_shader.cpp

#include <duk_rhi/vulkan/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_flags.h>
#include <duk_rhi/vulkan/vulkan_vertex.h>
#include <duk_rhi/shader_reflection.h>

#include <stdexcept>

namespace duk::rhi {

VkShaderStageFlagBits convert_module(ShaderModule::Bits module) {
    VkShaderStageFlagBits stage;
    switch (module) {
        case ShaderModule::VERTEX:
            stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderModule::TESSELLATION_CONTROL:
            stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            break;
        case ShaderModule::TESSELLATION_EVALUATION:
            stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            break;
        case ShaderModule::GEOMETRY:
            stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        case ShaderModule::FRAGMENT:
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderModule::COMPUTE:
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        default:
            throw std::runtime_error("tried to convert unsupported ShaderModule");
    }
    return stage;
}

VkShaderStageFlags convert_module_mask(ShaderModule::Mask moduleMask) {
    return convert_flags<ShaderModule>(moduleMask, convert_module);
}

VulkanShader::VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo)
    : m_device(shaderCreateInfo.device)
    , m_hash(shaderCreateInfo.shaderDataSource->hash())
    , m_moduleMask(shaderCreateInfo.shaderDataSource->module_mask()) {
    auto* shaderDataSource = shaderCreateInfo.shaderDataSource;

    if (shaderDataSource->module_mask() == 0) {
        throw std::runtime_error("VulkanShader: ShaderDataSource has no valid modules");
    }

    m_bindingLayout = shaderDataSource->binding_layout();
    m_vertexLayout = shaderDataSource->vertex_layout();

    // Remap bindings to a single descriptor set
    m_bindingRemapTable.reserve(m_bindingLayout.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(m_bindingLayout.size()); i++) {
        m_bindingRemapTable.push_back({0u, i});
    }

    ShaderReflection reflection(shaderDataSource->shader_modules());
    auto patchedModules = reflection.remap_bindings(m_bindingLayout);

    // -------------------------------------------------------------------------
    // Create VkShaderModule objects from the patched bytecodes.
    // -------------------------------------------------------------------------
    for (auto& [stage, patchedCode]: patchedModules) {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = patchedCode.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(patchedCode.data());

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        if (vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("VulkanShader: failed to create VkShaderModule");
        }

        auto [it, inserted] = m_shaderModules.emplace(stage, shaderModule);
        if (!inserted) {
            throw std::runtime_error("VulkanShader: duplicate shader module for stage");
        }
    }

    // -------------------------------------------------------------------------
    // Vertex input attribute and binding descriptions.
    // -------------------------------------------------------------------------
    m_inputAttributes.reserve(m_vertexLayout.size());
    m_inputBindings.reserve(m_vertexLayout.size());

    for (auto& format: m_vertexLayout) {
        VkVertexInputAttributeDescription inputAttributeDescription = {};
        inputAttributeDescription.binding = static_cast<uint32_t>(m_inputAttributes.size());
        inputAttributeDescription.location = inputAttributeDescription.binding;
        inputAttributeDescription.offset = 0;
        inputAttributeDescription.format = convert_vertex_attribute_format(format);
        m_inputAttributes.push_back(inputAttributeDescription);

        VkVertexInputBindingDescription inputBindingDescription = {};
        inputBindingDescription.binding = inputAttributeDescription.binding;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        inputBindingDescription.stride = VertexInput::size_of(format);
        m_inputBindings.push_back(inputBindingDescription);
    }
}

VulkanShader::~VulkanShader() {
    for (auto& [moduleType, module]: m_shaderModules) {
        vkDestroyShaderModule(m_device, module, nullptr);
    }
}

const std::vector<VkVertexInputAttributeDescription>& VulkanShader::input_attributes() const {
    return m_inputAttributes;
}

const std::vector<VkVertexInputBindingDescription>& VulkanShader::input_bindings() const {
    return m_inputBindings;
}

const std::unordered_map<ShaderModule::Bits, VkShaderModule>& VulkanShader::shader_modules() const {
    return m_shaderModules;
}

const BindingLocation& VulkanShader::binding_location(uint32_t logicalIndex) const {
    return m_bindingRemapTable.at(logicalIndex);
}

const ShaderBindingLayout& VulkanShader::binding_layout() const {
    return m_bindingLayout;
}

const VertexLayout& VulkanShader::vertex_layout() const {
    return m_vertexLayout;
}

hash::Hash VulkanShader::hash() const {
    return m_hash;
}

bool VulkanShader::is_graphics_shader() const {
    return m_moduleMask & (ShaderModule::VERTEX | ShaderModule::FRAGMENT);
}

bool VulkanShader::is_compute_shader() const {
    return m_moduleMask & ShaderModule::COMPUTE;
}

}// namespace duk::rhi
