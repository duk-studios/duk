/// 17/05/2023
/// vulkan_shader.cpp

#include <duk_rhi/vulkan/pipeline/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_descriptor_set.h>
#include <duk_rhi/vulkan/vulkan_flags.h>
#include <duk_rhi/vulkan/vulkan_vertex.h>

#include <stdexcept>
#include <duk_rhi/pipeline/shader_reflection.h>

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
    auto shaderDataSource = shaderCreateInfo.shaderDataSource;
    if (shaderDataSource->valid_graphics_shader()) {
        ShaderModule::Bits modules[] = {ShaderModule::VERTEX, ShaderModule::TESSELLATION_CONTROL, ShaderModule::TESSELLATION_EVALUATION, ShaderModule::GEOMETRY, ShaderModule::FRAGMENT};

        for (auto module: modules) {
            create_shader_module(module, shaderDataSource);
        }

    } else if (shaderDataSource->valid_compute_shader()) {
        const bool created = create_shader_module(ShaderModule::COMPUTE, shaderDataSource);
        if (!created) {
            throw std::runtime_error("failed to create compute VkShaderModule");
        }
    } else {
        throw std::runtime_error("invalid ShaderDataSource provided to VulkanShader");
    }

    m_descriptorSetLayouts.reserve(m_descriptorSetDescriptions.size());
    for (auto& descriptorSetDescription: m_descriptorSetDescriptions) {
        m_descriptorSetLayouts.push_back(shaderCreateInfo.descriptorSetLayoutCache->get_layout(descriptorSetDescription));
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    auto result = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkPipelineLayout");
    }

    m_inputAttributes.reserve(m_vertexLayout.size());
    m_inputBindings.reserve(m_vertexLayout.size());

    for (auto& format: m_vertexLayout) {
        VkVertexInputAttributeDescription inputAttributeDescription = {};
        inputAttributeDescription.binding = m_inputAttributes.size();
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
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
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

const std::vector<VkDescriptorSetLayout>& VulkanShader::descriptor_set_layouts() const {
    return m_descriptorSetLayouts;
}

const VkPipelineLayout& VulkanShader::pipeline_layout() const {
    return m_pipelineLayout;
}

const DescriptorSetDescription& VulkanShader::descriptor_set_description(uint32_t set) const {
    return m_descriptorSetDescriptions.at(set);
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
    return m_moduleMask & (ShaderModule::COMPUTE);
}

bool VulkanShader::create_shader_module(ShaderModule::Bits type, const ShaderDataSource* shaderDataSource) {
    if (!shaderDataSource->has_module(type)) {
        return false;
    }

    const auto& code = shaderDataSource->shader_module_spir_v_code(type);

    reflect_descriptor_sets(code.data(), code.size(), type, m_descriptorSetDescriptions);

    if (type == ShaderModule::VERTEX) {
        reflect_vertex_layout(code.data(), code.size(), m_vertexLayout);
    }

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = (const uint32_t*)code.data();

    VkShaderModule shaderModule;

    auto result = vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkShaderModule");
    }

    auto [it, inserted] = m_shaderModules.emplace(type, shaderModule);
    if (!inserted) {
        throw std::runtime_error("failed to insert shader module into unordered_map");
    }

    return true;
}

}// namespace duk::rhi