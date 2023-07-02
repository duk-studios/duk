/// 17/05/2023
/// vulkan_pipeline.cpp

#include <duk_rhi/vulkan/pipeline/vulkan_graphics_pipeline.h>
#include <duk_rhi/vulkan/pipeline/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_render_pass.h>
#include <duk_rhi/vulkan/vulkan_flags.h>

namespace duk::rhi {

VkCullModeFlagBits convert_cull_mode(GraphicsPipeline::CullMode::Bits cullModeBit) {
    VkCullModeFlagBits converted;
    switch (cullModeBit) {
        case GraphicsPipeline::CullMode::NONE: converted = VK_CULL_MODE_NONE; break;
        case GraphicsPipeline::CullMode::FRONT: converted = VK_CULL_MODE_FRONT_BIT; break;
        case GraphicsPipeline::CullMode::BACK: converted = VK_CULL_MODE_BACK_BIT; break;
        default:
            throw std::invalid_argument("unhandled Pipeline::CullMode for Vulkan");
    }
    return converted;
}

VkCullModeFlags convert_cull_mode_mask(GraphicsPipeline::CullMode::Mask cullModeMask) {
    return convert_flags<GraphicsPipeline::CullMode>(cullModeMask, convert_cull_mode);
}

VkBlendOp convert_blend_op(GraphicsPipeline::Blend::Operator blendOperator) {
    VkBlendOp converted;
    switch (blendOperator) {
        case GraphicsPipeline::Blend::Operator::ADD: converted = VK_BLEND_OP_ADD; break;
        case GraphicsPipeline::Blend::Operator::SUBTRACT: converted = VK_BLEND_OP_SUBTRACT; break;
        case GraphicsPipeline::Blend::Operator::REVERSE_SUBTRACT: converted = VK_BLEND_OP_REVERSE_SUBTRACT; break;
        case GraphicsPipeline::Blend::Operator::MIN: converted = VK_BLEND_OP_MIN; break;
        case GraphicsPipeline::Blend::Operator::MAX: converted = VK_BLEND_OP_MAX; break;
        default:
            throw std::invalid_argument("unhandled Pipeline::Blend::Operator for Vulkan");
    }
    return converted;
}

VkBlendFactor convert_blend_factor(GraphicsPipeline::Blend::Factor blendFactor) {
    VkBlendFactor converted;
    switch (blendFactor) {
        case GraphicsPipeline::Blend::Factor::ZERO: converted = VK_BLEND_FACTOR_ZERO; break;
        case GraphicsPipeline::Blend::Factor::ONE: converted = VK_BLEND_FACTOR_ONE; break;
        case GraphicsPipeline::Blend::Factor::SRC_COLOR: converted = VK_BLEND_FACTOR_SRC_COLOR; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; break;
        case GraphicsPipeline::Blend::Factor::DST_COLOR: converted = VK_BLEND_FACTOR_DST_COLOR; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_DST_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; break;
        case GraphicsPipeline::Blend::Factor::SRC_ALPHA: converted = VK_BLEND_FACTOR_SRC_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::DST_ALPHA: converted = VK_BLEND_FACTOR_DST_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_DST_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::CONSTANT_COLOR: converted = VK_BLEND_FACTOR_CONSTANT_COLOR; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_CONSTANT_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; break;
        case GraphicsPipeline::Blend::Factor::CONSTANT_ALPHA: converted = VK_BLEND_FACTOR_CONSTANT_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_CONSTANT_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::SRC_ALPHA_SATURATE: converted = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; break;
        case GraphicsPipeline::Blend::Factor::SRC1_COLOR: converted = VK_BLEND_FACTOR_SRC1_COLOR; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC1_COLOR: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; break;
        case GraphicsPipeline::Blend::Factor::SRC1_ALPHA: converted = VK_BLEND_FACTOR_SRC1_ALPHA; break;
        case GraphicsPipeline::Blend::Factor::ONE_MINUS_SRC1_ALPHA: converted = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; break;
        default:
            throw std::invalid_argument("unhandled Pipeline::Blend::Factor for Vulkan");
    }
    return converted;
}

VkPrimitiveTopology convert_topology(GraphicsPipeline::Topology topology) {
    VkPrimitiveTopology converted;
    switch (topology) {
        case GraphicsPipeline::Topology::POINT_LIST: converted = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case GraphicsPipeline::Topology::LINE_LIST: converted = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
        case GraphicsPipeline::Topology::LINE_STRIP: converted = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
        case GraphicsPipeline::Topology::TRIANGLE_LIST: converted = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case GraphicsPipeline::Topology::TRIANGLE_STRIP: converted = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        case GraphicsPipeline::Topology::TRIANGLE_FAN: converted = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
        case GraphicsPipeline::Topology::LINE_LIST_WITH_ADJACENCY: converted = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY; break;
        case GraphicsPipeline::Topology::LINE_STRIP_WITH_ADJACENCY: converted = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY; break;
        case GraphicsPipeline::Topology::TRIANGLE_LIST_WITH_ADJACENCY: converted = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY; break;
        case GraphicsPipeline::Topology::TRIANGLE_STRIP_WITH_ADJACENCY: converted = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; break;
        case GraphicsPipeline::Topology::PATCH_LIST: converted = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
        default:
            throw std::invalid_argument("unhandled Pipeline::Topology for Vulkan");
    }
    return converted;
}

VkPolygonMode convert_fill_mode(GraphicsPipeline::FillMode fillMode) {
    VkPolygonMode converted;
    switch (fillMode) {
        case GraphicsPipeline::FillMode::FILL: converted = VK_POLYGON_MODE_FILL; break;
        case GraphicsPipeline::FillMode::LINE: converted = VK_POLYGON_MODE_LINE; break;
        case GraphicsPipeline::FillMode::POINT: converted = VK_POLYGON_MODE_POINT; break;
        default:
            throw std::invalid_argument("unhandled Pipeline::FillMode for Vulkan");
    }
    return converted;
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo& pipelineCreateInfo) :
    m_device(pipelineCreateInfo.device),
    m_shader(pipelineCreateInfo.shader),
    m_renderPass(pipelineCreateInfo.renderPass),
    m_viewport(pipelineCreateInfo.viewport),
    m_scissor(pipelineCreateInfo.scissor),
    m_cullModeMask(pipelineCreateInfo.cullModeMask),
    m_blend(pipelineCreateInfo.blend),
    m_topology(pipelineCreateInfo.topology),
    m_fillMode(pipelineCreateInfo.fillMode),
    m_depthTesting(pipelineCreateInfo.depthTesting),
    m_hash(duk::hash::kUndefinedHash) {
    if (!m_shader->is_graphics_shader()) {
        throw std::invalid_argument("invalid shader type for GraphicsPipeline");
    }
    create(pipelineCreateInfo.imageCount);
    update_hash();
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    clean();
}

void VulkanGraphicsPipeline::create(uint32_t imageCount) {
    m_pipelineHashes.resize(imageCount, duk::hash::kUndefinedHash);
    m_pipelines.resize(imageCount);
}

void VulkanGraphicsPipeline::clean(uint32_t imageIndex) {
    auto& pipeline = m_pipelines[imageIndex];
    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, pipeline, nullptr);
        pipeline = VK_NULL_HANDLE;
    }
}

void VulkanGraphicsPipeline::clean() {
    for (int i = 0; i < m_pipelines.size(); i++) {
        clean(i);
    }
    m_pipelines.clear();
    m_pipelineHashes.clear();
}

void VulkanGraphicsPipeline::update(uint32_t imageIndex) {
    auto& pipelineHash = m_pipelineHashes[imageIndex];
    if (pipelineHash == m_hash) {
        return;
    }
    pipelineHash = m_hash;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    {
        auto& shaderModules = m_shader->shader_modules();
        shaderStages.reserve(shaderModules.size());

        for (auto&[moduleType, module] : shaderModules) {

            VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
            shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.stage = convert_module(moduleType);
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
    inputAssembly.topology = convert_topology(m_topology);
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
    rasterizer.polygonMode = convert_fill_mode(m_fillMode);
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = convert_cull_mode_mask(m_cullModeMask);
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
        blendAttachmentState.srcColorBlendFactor = convert_blend_factor(m_blend.srcColorBlendFactor);
        blendAttachmentState.dstColorBlendFactor = convert_blend_factor(m_blend.dstColorBlendFactor);
        blendAttachmentState.colorBlendOp = convert_blend_op(m_blend.colorBlendOp);
        blendAttachmentState.srcAlphaBlendFactor = convert_blend_factor(m_blend.srcAlphaBlendFactor);
        blendAttachmentState.dstAlphaBlendFactor = convert_blend_factor(m_blend.dstAlphaBlendFactor);
        blendAttachmentState.alphaBlendOp = convert_blend_op(m_blend.alphaBlendOp);
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

    auto& pipeline = m_pipelines[imageIndex];

    auto result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkPipeline");
    }
}

const VkPipeline& VulkanGraphicsPipeline::handle(uint32_t imageIndex) const {
    return m_pipelines[imageIndex];
}

VkPipelineLayout VulkanGraphicsPipeline::pipeline_layout() const {
    return m_shader->pipeline_layout();
}

void VulkanGraphicsPipeline::set_viewport(const GraphicsPipeline::Viewport& viewport) {
    m_viewport = viewport;
}

const GraphicsPipeline::Viewport& VulkanGraphicsPipeline::viewport() const {
    return m_viewport;
}

void VulkanGraphicsPipeline::set_scissor(const GraphicsPipeline::Scissor& scissor) {
    m_scissor = scissor;
}

const GraphicsPipeline::Scissor& VulkanGraphicsPipeline::scissor() const {
    return m_scissor;
}

void VulkanGraphicsPipeline::set_blend(const GraphicsPipeline::Blend& blend) {
    m_blend = blend;
}

const GraphicsPipeline::Blend& VulkanGraphicsPipeline::blend() const {
    return m_blend;
}

void VulkanGraphicsPipeline::set_cull_mode(GraphicsPipeline::CullMode::Mask cullModeMask) {
    m_cullModeMask = cullModeMask;
}

GraphicsPipeline::CullMode::Mask VulkanGraphicsPipeline::cull_mode() {
    return m_cullModeMask;
}

void VulkanGraphicsPipeline::set_topology(GraphicsPipeline::Topology topology) {
    m_topology = topology;
}

GraphicsPipeline::Topology VulkanGraphicsPipeline::topology() const {
    return m_topology;
}

void VulkanGraphicsPipeline::set_fill_mode(GraphicsPipeline::FillMode fillMode) {
    m_fillMode = fillMode;
}

GraphicsPipeline::FillMode VulkanGraphicsPipeline::fill_mode() const {
    return m_fillMode;
}

void VulkanGraphicsPipeline::flush() {
    update_hash();
}

hash::Hash VulkanGraphicsPipeline::hash() const {
    return m_hash;
}

void VulkanGraphicsPipeline::update_hash() {
    m_hash = 0;
    duk::hash::hash_combine(m_hash, m_viewport.offset);
    duk::hash::hash_combine(m_hash, m_viewport.extent);
    duk::hash::hash_combine(m_hash, m_blend.alphaBlendOp);
    duk::hash::hash_combine(m_hash, m_blend.colorBlendOp);
    duk::hash::hash_combine(m_hash, m_blend.dstAlphaBlendFactor);
    duk::hash::hash_combine(m_hash, m_blend.srcAlphaBlendFactor);
    duk::hash::hash_combine(m_hash, m_blend.dstColorBlendFactor);
    duk::hash::hash_combine(m_hash, m_blend.srcColorBlendFactor);
    duk::hash::hash_combine(m_hash, m_blend.enabled);
    duk::hash::hash_combine(m_hash, m_scissor.extent);
    duk::hash::hash_combine(m_hash, m_scissor.offset);
    duk::hash::hash_combine(m_hash, m_shader->hash());
    duk::hash::hash_combine(m_hash, m_renderPass->hash());
    duk::hash::hash_combine(m_hash, m_cullModeMask);
    duk::hash::hash_combine(m_hash, m_depthTesting);
}

}