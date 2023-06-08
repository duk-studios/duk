/// 17/05/2023
/// vulkan_pipeline.cpp

#include <duk_renderer/vulkan/pipeline/vulkan_pipeline.h>
#include <duk_renderer/vulkan/pipeline/vulkan_shader.h>
#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/vulkan_flags.h>

namespace duk::renderer {

namespace vk {

VkCullModeFlagBits convert_cull_mode(Pipeline::CullMode::Bits cullModeBit) {
    VkCullModeFlagBits converted;
    switch (cullModeBit) {
        case Pipeline::CullMode::NONE: converted = VK_CULL_MODE_NONE; break;
        case Pipeline::CullMode::FRONT: converted = VK_CULL_MODE_FRONT_BIT; break;
        case Pipeline::CullMode::BACK: converted = VK_CULL_MODE_BACK_BIT; break;
    }
    return converted;
}

VkCullModeFlags convert_cull_mode_mask(Pipeline::CullMode::Mask cullModeMask) {
    return convert_flags<Pipeline::CullMode>(cullModeMask, convert_cull_mode);
}

VkBlendOp convert_blend_op(Pipeline::Blend::Operator blendOperator) {
    VkBlendOp converted;
    switch (blendOperator) {
        case Pipeline::Blend::Operator::ADD: converted = VK_BLEND_OP_ADD; break;
        case Pipeline::Blend::Operator::SUBTRACT: converted = VK_BLEND_OP_SUBTRACT; break;
        case Pipeline::Blend::Operator::REVERSE_SUBTRACT: converted = VK_BLEND_OP_REVERSE_SUBTRACT; break;
        case Pipeline::Blend::Operator::MIN: converted = VK_BLEND_OP_MIN; break;
        case Pipeline::Blend::Operator::MAX: converted = VK_BLEND_OP_MAX; break;
    }
    return converted;
}

VkBlendFactor convert_blend_factor(Pipeline::Blend::Factor blendFactor) {
    VkBlendFactor converted;
    switch (blendFactor) {
        case Pipeline::Blend::Factor::ZERO: converted = VK_BLEND_FACTOR_ZERO; break;
        case Pipeline::Blend::Factor::ONE: converted = VK_BLEND_FACTOR_ONE; break;
        case Pipeline::Blend::Factor::SRC_COLOR: converted = VK_BLEND_FACTOR_SRC_COLOR; break;
        case Pipeline::Blend::Factor::ONE_MINUS_SRC_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; break;
        case Pipeline::Blend::Factor::DST_COLOR: converted = VK_BLEND_FACTOR_DST_COLOR; break;
        case Pipeline::Blend::Factor::ONE_MINUS_DST_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; break;
        case Pipeline::Blend::Factor::SRC_ALPHA: converted = VK_BLEND_FACTOR_SRC_ALPHA; break;
        case Pipeline::Blend::Factor::ONE_MINUS_SRC_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
        case Pipeline::Blend::Factor::DST_ALPHA: converted = VK_BLEND_FACTOR_DST_ALPHA; break;
        case Pipeline::Blend::Factor::ONE_MINUS_DST_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
        case Pipeline::Blend::Factor::CONSTANT_COLOR: converted = VK_BLEND_FACTOR_CONSTANT_COLOR; break;
        case Pipeline::Blend::Factor::ONE_MINUS_CONSTANT_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; break;
        case Pipeline::Blend::Factor::CONSTANT_ALPHA: converted = VK_BLEND_FACTOR_CONSTANT_ALPHA; break;
        case Pipeline::Blend::Factor::ONE_MINUS_CONSTANT_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
        case Pipeline::Blend::Factor::SRC_ALPHA_SATURATE: converted = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; break;
        case Pipeline::Blend::Factor::SRC1_COLOR: converted = VK_BLEND_FACTOR_SRC1_COLOR; break;
        case Pipeline::Blend::Factor::ONE_MINUS_SRC1_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; break;
        case Pipeline::Blend::Factor::SRC1_ALPHA: converted = VK_BLEND_FACTOR_SRC1_ALPHA; break;
        case Pipeline::Blend::Factor::ONE_MINUS_SRC1_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; break;
    }
    return converted;
}

}

VulkanPipeline::VulkanPipeline(const VulkanPipelineCreateInfo& pipelineCreateInfo) :
    m_device(pipelineCreateInfo.device),
    m_shader(pipelineCreateInfo.shader),
    m_renderPass(pipelineCreateInfo.renderPass),
    m_viewport(pipelineCreateInfo.viewport),
    m_scissor(pipelineCreateInfo.scissor),
    m_cullModeMask(pipelineCreateInfo.cullModeMask),
    m_blend(pipelineCreateInfo.blend),
    m_depthTesting(pipelineCreateInfo.depthTesting) {
    create(pipelineCreateInfo.imageCount);

}

VulkanPipeline::~VulkanPipeline() {
    clean();
}

void VulkanPipeline::create_graphics_pipeline(uint32_t imageCount) {

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    {
        auto& shaderModules = m_shader->shader_modules();
        shaderStages.reserve(shaderModules.size());

        for (auto&[moduleType, module] : shaderModules) {

            VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
            shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.stage = vk::convert_module(moduleType);
            shaderStageCreateInfo.module = module;
            shaderStageCreateInfo.pName = "main";

            shaderStages.push_back(shaderStageCreateInfo);
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    {
        auto& inputAttributes = m_shader->input_attributes();
        auto& inputBindings = m_shader->input_bindings();
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = inputBindings.size();
        vertexInputInfo.pVertexBindingDescriptions = inputBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = inputAttributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = inputAttributes.data();
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = m_viewport.offset.x;
    viewport.y = m_viewport.offset.y;
    viewport.width = m_viewport.extent.x;
    viewport.height = m_viewport.extent.y;
    viewport.minDepth = m_viewport.minDepth;
    viewport.maxDepth = m_viewport.maxDepth;

    VkRect2D scissor = {};
    scissor.offset = { m_scissor.offset.x, m_scissor.offset.y };
    scissor.extent = { m_scissor.extent.x, m_scissor.extent.y };

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::convert_cull_mode_mask(m_cullModeMask);
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachmentState = {};
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    if (m_blend.enabled) {
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.srcColorBlendFactor = vk::convert_blend_factor(m_blend.srcColorBlendFactor);
        blendAttachmentState.dstColorBlendFactor = vk::convert_blend_factor(m_blend.dstColorBlendFactor);
        blendAttachmentState.colorBlendOp = vk::convert_blend_op(m_blend.colorBlendOp);
        blendAttachmentState.srcAlphaBlendFactor = vk::convert_blend_factor(m_blend.srcAlphaBlendFactor);
        blendAttachmentState.dstAlphaBlendFactor = vk::convert_blend_factor(m_blend.dstAlphaBlendFactor);
        blendAttachmentState.alphaBlendOp = vk::convert_blend_op(m_blend.alphaBlendOp);
    }
    else {
        blendAttachmentState.blendEnable = VK_FALSE;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    }

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(m_renderPass->color_attachment_count(), blendAttachmentState);

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = blendAttachments.size();
    colorBlending.pAttachments = blendAttachments.data();

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (m_depthTesting) {
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }
    else {
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.layout = m_shader->pipeline_layout();
    graphicsPipelineCreateInfo.renderPass = m_renderPass->handle();
    graphicsPipelineCreateInfo.stageCount = shaderStages.size();
    graphicsPipelineCreateInfo.pStages = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    graphicsPipelineCreateInfo.pViewportState = &viewportState;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizer;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampling;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlending;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;

    std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfos(imageCount, graphicsPipelineCreateInfo);
    m_pipelines.resize(3);

    auto result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, imageCount, graphicsPipelineCreateInfos.data(), nullptr, m_pipelines.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkPipeline");
    }

    m_pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void VulkanPipeline::create(uint32_t imageCount) {
    if (m_shader->is_graphics_shader()) {
        create_graphics_pipeline(imageCount);
    }
    else {
        throw std::invalid_argument("Unsupported shader type");
    }
}

void VulkanPipeline::clean(uint32_t imageIndex) {
    auto& pipeline = m_pipelines[imageIndex];
    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, pipeline, nullptr);
        pipeline = VK_NULL_HANDLE;
    }
}

void VulkanPipeline::clean() {
    for (int i = 0; i < m_pipelines.size(); i++) {
        clean(i);
    }
    m_pipelines.clear();
}

const VkPipeline& VulkanPipeline::handle(uint32_t imageIndex) const {
    return m_pipelines[imageIndex];
}

VkPipelineBindPoint VulkanPipeline::bind_point() const {
    return m_pipelineBindPoint;
}

VkPipelineLayout VulkanPipeline::pipeline_layout() const {
    return m_shader->pipeline_layout();
}

}