/// static_render_state_vk.cpp

#include <duk_rhi/vulkan/vulkan_static_render_state.h>
#include <duk_rhi/vulkan/vulkan_shader.h>
#include <duk_rhi/vulkan/vulkan_flags.h>
#include <duk_rhi/vulkan/vulkan_image.h>

#include <duk_hash/hash.h>

#include <stdexcept>
#include <vector>

namespace duk::rhi {

namespace detail {

static VkCullModeFlagBits convert_cull_mode(PipelineState::CullMode::Bits bit) {
    switch (bit) {
        case PipelineState::CullMode::NONE:  return VK_CULL_MODE_NONE;
        case PipelineState::CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT;
        case PipelineState::CullMode::BACK:  return VK_CULL_MODE_BACK_BIT;
        default: throw std::invalid_argument("unhandled PipelineState::CullMode::Bits for Vulkan");
    }
}

static VkCullModeFlags convert_cull_mode_mask(PipelineState::CullMode::Mask mask) {
    return convert_flags<PipelineState::CullMode>(mask, convert_cull_mode);
}

static VkBlendOp convert_blend_op(PipelineState::Blend::Operator op) {
    switch (op) {
        case PipelineState::Blend::Operator::ADD:              return VK_BLEND_OP_ADD;
        case PipelineState::Blend::Operator::SUBTRACT:         return VK_BLEND_OP_SUBTRACT;
        case PipelineState::Blend::Operator::REVERSE_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case PipelineState::Blend::Operator::MIN:              return VK_BLEND_OP_MIN;
        case PipelineState::Blend::Operator::MAX:              return VK_BLEND_OP_MAX;
        default: throw std::invalid_argument("unhandled PipelineState::Blend::Operator for Vulkan");
    }
}

static VkBlendFactor convert_blend_factor(PipelineState::Blend::Factor factor) {
    switch (factor) {
        case PipelineState::Blend::Factor::ZERO:                  return VK_BLEND_FACTOR_ZERO;
        case PipelineState::Blend::Factor::ONE:                   return VK_BLEND_FACTOR_ONE;
        case PipelineState::Blend::Factor::SRC_COLOR:             return VK_BLEND_FACTOR_SRC_COLOR;
        case PipelineState::Blend::Factor::ONE_MINUS_SRC_COLOR:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case PipelineState::Blend::Factor::DST_COLOR:             return VK_BLEND_FACTOR_DST_COLOR;
        case PipelineState::Blend::Factor::ONE_MINUS_DST_COLOR:   return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case PipelineState::Blend::Factor::SRC_ALPHA:             return VK_BLEND_FACTOR_SRC_ALPHA;
        case PipelineState::Blend::Factor::ONE_MINUS_SRC_ALPHA:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case PipelineState::Blend::Factor::DST_ALPHA:             return VK_BLEND_FACTOR_DST_ALPHA;
        case PipelineState::Blend::Factor::ONE_MINUS_DST_ALPHA:   return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case PipelineState::Blend::Factor::CONSTANT_COLOR:        return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case PipelineState::Blend::Factor::ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case PipelineState::Blend::Factor::CONSTANT_ALPHA:        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case PipelineState::Blend::Factor::ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case PipelineState::Blend::Factor::SRC_ALPHA_SATURATE:    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case PipelineState::Blend::Factor::SRC1_COLOR:            return VK_BLEND_FACTOR_SRC1_COLOR;
        case PipelineState::Blend::Factor::ONE_MINUS_SRC1_COLOR:  return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case PipelineState::Blend::Factor::SRC1_ALPHA:            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case PipelineState::Blend::Factor::ONE_MINUS_SRC1_ALPHA:  return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default: throw std::invalid_argument("unhandled PipelineState::Blend::Factor for Vulkan");
    }
}

static VkPrimitiveTopology convert_topology(PipelineState::Topology topology) {
    switch (topology) {
        case PipelineState::Topology::POINT_LIST:                    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PipelineState::Topology::LINE_LIST:                     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PipelineState::Topology::LINE_STRIP:                    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PipelineState::Topology::TRIANGLE_LIST:                 return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PipelineState::Topology::TRIANGLE_STRIP:                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PipelineState::Topology::TRIANGLE_FAN:                  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case PipelineState::Topology::LINE_LIST_WITH_ADJACENCY:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
        case PipelineState::Topology::LINE_STRIP_WITH_ADJACENCY:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
        case PipelineState::Topology::TRIANGLE_LIST_WITH_ADJACENCY:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case PipelineState::Topology::TRIANGLE_STRIP_WITH_ADJACENCY: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        case PipelineState::Topology::PATCH_LIST:                    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        default: throw std::invalid_argument("unhandled PipelineState::Topology for Vulkan");
    }
}

static VkPolygonMode convert_fill_mode(PipelineState::FillMode fillMode) {
    switch (fillMode) {
        case PipelineState::FillMode::FILL:  return VK_POLYGON_MODE_FILL;
        case PipelineState::FillMode::LINE:  return VK_POLYGON_MODE_LINE;
        case PipelineState::FillMode::POINT: return VK_POLYGON_MODE_POINT;
        default: throw std::invalid_argument("unhandled PipelineState::FillMode for Vulkan");
    }
}

static bool is_depth_attachment(const VulkanImage& image) {
    return (image.image_aspect() & VK_IMAGE_ASPECT_DEPTH_BIT) != 0;
}

}// namespace detail

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

VulkanStaticRenderState::VulkanStaticRenderState(const VulkanStaticRenderStateCreateInfo& createInfo)
    : m_device(createInfo.device)
    , m_deletionQueue(createInfo.deletionQueue) {
}

VulkanStaticRenderState::~VulkanStaticRenderState() {
    // Destroy all cached Vulkan objects. GPU-idle is assumed at this point.
    for (auto& [key, pipeline] : m_pipelineCache) {
        vkDestroyPipeline(m_device, pipeline, nullptr);
    }
    for (auto& [key, framebuffer] : m_framebufferCache) {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    for (auto& [key, rp] : m_renderPassCache) {
        vkDestroyRenderPass(m_device, rp.handle, nullptr);
    }
}

// -----------------------------------------------------------------------------
// VulkanStaticRenderState interface
// -----------------------------------------------------------------------------

void VulkanStaticRenderState::begin(VkCommandBuffer commandBuffer,
                                    const VulkanFrameBuffer& frameBuffer,
                                    VkAttachmentLoadOp loadOp,
                                    VkAttachmentStoreOp storeOp,
                                    const glm::vec4& clearColor) {
    auto [renderPass, colorAttachmentCount] = get_or_create_render_pass(frameBuffer, loadOp, storeOp);
    auto framebuffer = get_or_create_framebuffer(renderPass, frameBuffer);

    std::array<VkClearValue, 8> clearValues;
    uint32_t clearValueCount = 0;
    if (loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
        for (uint32_t i = 0; i < colorAttachmentCount && i < clearValues.size() - 1; ++i) {
            clearValues[i].color = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};
            clearValueCount++;
        }
        if (clearValues.size() > colorAttachmentCount) {
            clearValues[colorAttachmentCount].depthStencil = {1.0f, 0};
            clearValueCount++;
        }
    }

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.offset = {0, 0};
    beginInfo.renderArea.extent = {frameBuffer.width(), frameBuffer.height()};
    beginInfo.clearValueCount = clearValueCount;
    beginInfo.pClearValues = clearValueCount > 0 ? clearValues.data() : nullptr;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    m_activeRenderPass = renderPass;
    m_activeFramebuffer = framebuffer;
    m_activeColorAttachmentCount = colorAttachmentCount;
    m_activePipeline = VK_NULL_HANDLE; // Force pipeline rebind after render pass change.

    // The render pass will implicitly transition each attachment to its declared
    // finalLayout by the time vkCmdEndRenderPass returns. Update tracked layouts now
    // so that any post-pass barriers (e.g. PRESENT_SRC_KHR in flush()) use the
    // correct srcAccessMask.
    for (uint32_t i = 0; i < frameBuffer.attachment_count(); ++i) {
        const auto* img = static_cast<const VulkanImage*>(frameBuffer.at(i));
        if (detail::is_depth_attachment(*img)) {
            img->set_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        } else {
            img->set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
    }
}

void VulkanStaticRenderState::bind_pipeline(VkCommandBuffer commandBuffer, const VulkanShader& shader, const PipelineState& state) {
    if (!m_activeRenderPass || !m_activeFramebuffer) {
        throw std::logic_error("No active render pass when binding pipeline in VulkanStaticRenderState");
    }

    const auto pipeline = get_or_create_pipeline(m_activeRenderPass, m_activeColorAttachmentCount, shader, state);
    if (!pipeline) {
        throw std::runtime_error("Failed to create pipeline in VulkanStaticRenderState");
    }
    if (pipeline != m_activePipeline) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        m_activePipeline = pipeline;
    }
}

void VulkanStaticRenderState::end(VkCommandBuffer commandBuffer) {
    if (m_activeRenderPass != VK_NULL_HANDLE) {
        vkCmdEndRenderPass(commandBuffer);
    }
    m_activeRenderPass = VK_NULL_HANDLE;
    m_activeFramebuffer = VK_NULL_HANDLE;
    m_activePipeline = VK_NULL_HANDLE;
}

// -----------------------------------------------------------------------------
// Cache helpers
// -----------------------------------------------------------------------------

VulkanStaticRenderState::CachedRenderPass VulkanStaticRenderState::get_or_create_render_pass(
        const VulkanFrameBuffer& frameBuffer,
        VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp) {
    auto count = frameBuffer.attachment_count();

    // Build a key from each attachment's pixel format, depth flag, and the load/store ops.
    size_t key = 0;
    for (uint32_t i = 0; i < count; ++i) {
        auto* img = static_cast<const VulkanImage*>(frameBuffer.at(i));
        duk::hash::hash_combine(key, static_cast<uint32_t>(img->format()));
        duk::hash::hash_combine(key, detail::is_depth_attachment(*img));
    }
    duk::hash::hash_combine(key, static_cast<uint32_t>(loadOp));
    duk::hash::hash_combine(key, static_cast<uint32_t>(storeOp));

    auto it = m_renderPassCache.find(key);
    if (it != m_renderPassCache.end()) {
        return it->second;
    }

    std::vector<VkAttachmentDescription> attachments;
    attachments.reserve(count);

    std::vector<VkAttachmentReference> colorRefs;
    VkAttachmentReference depthRef = {};
    bool hasDepth = false;
    uint32_t colorCount = 0;

    for (uint32_t i = 0; i < count; ++i) {
        auto* img = static_cast<const VulkanImage*>(frameBuffer.at(i));
        const bool isDepth = detail::is_depth_attachment(*img);

        VkAttachmentDescription desc = {};
        desc.format = convert_pixel_format(img->format());
        desc.samples = VK_SAMPLE_COUNT_1_BIT;
        desc.loadOp = loadOp;
        desc.storeOp = storeOp;
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        if (isDepth) {
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthRef.attachment = static_cast<uint32_t>(attachments.size());
            depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            hasDepth = true;
        } else {
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkAttachmentReference colorRef = {};
            colorRef.attachment = static_cast<uint32_t>(attachments.size());
            colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorRefs.push_back(colorRef);
            ++colorCount;
        }

        attachments.push_back(desc);
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorRefs.size());
    subpass.pColorAttachments = colorRefs.data();
    if (hasDepth) {
        subpass.pDepthStencilAttachment = &depthRef;
    }

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    auto result = vkCreateRenderPass(m_device, &createInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("VulkanStaticRenderState: failed to create VkRenderPass");
    }

    CachedRenderPass cached;
    cached.handle = renderPass;
    cached.colorAttachmentCount = colorCount;
    m_renderPassCache.emplace(key, cached);
    return cached;
}

VkFramebuffer VulkanStaticRenderState::get_or_create_framebuffer(VkRenderPass renderPass,
                                                              const VulkanFrameBuffer& frameBuffer) {
    auto count = frameBuffer.attachment_count();

    size_t key = 0;
    duk::hash::hash_combine(key, reinterpret_cast<uintptr_t>(renderPass));
    for (uint32_t i = 0; i < count; ++i) {
        auto* img = static_cast<const VulkanImage*>(frameBuffer.at(i));
        duk::hash::hash_combine(key, reinterpret_cast<uintptr_t>(img->image_view()));
    }
    duk::hash::hash_combine(key, frameBuffer.width());
    duk::hash::hash_combine(key, frameBuffer.height());

    auto it = m_framebufferCache.find(key);
    if (it != m_framebufferCache.end()) {
        return it->second;
    }

    std::vector<VkImageView> imageViews;
    imageViews.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        auto* img = static_cast<const VulkanImage*>(frameBuffer.at(i));
        imageViews.push_back(img->image_view());
    }

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
    createInfo.pAttachments = imageViews.data();
    createInfo.width = frameBuffer.width();
    createInfo.height = frameBuffer.height();
    createInfo.layers = 1;

    VkFramebuffer framebuffer;
    auto result = vkCreateFramebuffer(m_device, &createInfo, nullptr, &framebuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("VulkanStaticRenderState: failed to create VkFramebuffer");
    }

    m_framebufferCache.emplace(key, framebuffer);
    return framebuffer;
}

VkPipeline VulkanStaticRenderState::get_or_create_pipeline(VkRenderPass renderPass,
                                                        uint32_t colorAttachmentCount,
                                                        const VulkanShader& shader,
                                                        const PipelineState& state) {
    size_t key = std::hash<PipelineState>{}(state);
    duk::hash::hash_combine(key, shader.hash());
    duk::hash::hash_combine(key, reinterpret_cast<uintptr_t>(renderPass));

    auto it = m_pipelineCache.find(key);
    if (it != m_pipelineCache.end()) {
        return it->second;
    }

    // ---- Shader stages ----
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    {
        auto& modules = shader.shader_modules();
        shaderStages.reserve(modules.size());
        for (auto& [moduleType, module] : modules) {
            VkPipelineShaderStageCreateInfo stageInfo = {};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = convert_module(moduleType);
            stageInfo.module = module;
            stageInfo.pName = "main";
            shaderStages.push_back(stageInfo);
        }
    }

    // ---- Vertex input ----
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    {
        auto& inputAttributes = shader.input_attributes();
        auto& inputBindings = shader.input_bindings();
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(inputBindings.size());
        vertexInputInfo.pVertexBindingDescriptions = inputBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = inputAttributes.data();
    }

    // ---- Input assembly ----
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = detail::convert_topology(state.topology);
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // ---- Viewport / scissor ----
    VkViewport viewport = {};
    viewport.x = state.viewport.offset.x;
    viewport.y = state.viewport.offset.y;
    viewport.width = state.viewport.extent.x;
    viewport.height = state.viewport.extent.y;
    viewport.minDepth = state.viewport.minDepth;
    viewport.maxDepth = state.viewport.maxDepth;

    VkRect2D scissor = {};
    scissor.offset = {state.scissor.offset.x, state.scissor.offset.y};
    scissor.extent = {state.scissor.extent.x, state.scissor.extent.y};

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // ---- Rasterization ----
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = detail::convert_fill_mode(state.fillMode);
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = detail::convert_cull_mode_mask(state.cullMode);
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // ---- Multisampling ----
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // ---- Colour blend (one attachment state replicated per colour output) ----
    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    if (state.blend.enabled) {
        blendAttachment.blendEnable = VK_TRUE;
        blendAttachment.srcColorBlendFactor = detail::convert_blend_factor(state.blend.srcColorBlendFactor);
        blendAttachment.dstColorBlendFactor = detail::convert_blend_factor(state.blend.dstColorBlendFactor);
        blendAttachment.colorBlendOp = detail::convert_blend_op(state.blend.colorBlendOp);
        blendAttachment.srcAlphaBlendFactor = detail::convert_blend_factor(state.blend.srcAlphaBlendFactor);
        blendAttachment.dstAlphaBlendFactor = detail::convert_blend_factor(state.blend.dstAlphaBlendFactor);
        blendAttachment.alphaBlendOp = detail::convert_blend_op(state.blend.alphaBlendOp);
    } else {
        blendAttachment.blendEnable = VK_FALSE;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments(colorAttachmentCount, blendAttachment);

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
    colorBlending.pAttachments = blendAttachments.data();

    // ---- Depth / stencil ----
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (state.depthTesting) {
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = state.blend.enabled ? VK_FALSE : VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    } else {
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }

    // ---- Assemble ----
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = shader.pipeline_layout();
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizer;
    pipelineCreateInfo.pMultisampleState = &multisampling;
    pipelineCreateInfo.pColorBlendState = &colorBlending;
    pipelineCreateInfo.pDepthStencilState = &depthStencil;

    VkPipeline pipeline;
    auto result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("VulkanStaticRenderState: failed to create VkPipeline");
    }

    m_pipelineCache.emplace(key, pipeline);
    return pipeline;
}

}// namespace duk::rhi

