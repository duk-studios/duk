/// 23/04/2023
/// vulkan_render_pass.cpp

#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/vulkan_image.h>

namespace duk::renderer {

VkAttachmentStoreOp convert_store_op(StoreOp storeOp) {
    VkAttachmentStoreOp converted;
    switch (storeOp){
        case StoreOp::STORE: converted = VK_ATTACHMENT_STORE_OP_STORE; break;
        case StoreOp::DONT_CARE: converted = VK_ATTACHMENT_STORE_OP_DONT_CARE; break;
        case StoreOp::NONE: converted = VK_ATTACHMENT_STORE_OP_NONE; break;
        default:
            throw std::invalid_argument("unhandled StoreOp for Vulkan");
    }
    return converted;
}

VkAttachmentLoadOp convert_load_op(LoadOp loadOp) {
    VkAttachmentLoadOp converted;
    switch (loadOp){
        case LoadOp::LOAD: converted = VK_ATTACHMENT_LOAD_OP_LOAD; break;
        case LoadOp::CLEAR: converted = VK_ATTACHMENT_LOAD_OP_CLEAR; break;
        case LoadOp::DONT_CARE: converted = VK_ATTACHMENT_LOAD_OP_DONT_CARE; break;
        case LoadOp::NONE: converted = VK_ATTACHMENT_LOAD_OP_NONE_EXT; break;
        default:
            throw std::invalid_argument("unhandled StoreOp for Vulkan");
    }
    return converted;
}

VkAttachmentDescription convert_attachment_description(const AttachmentDescription& attachmentDescription) {
    VkAttachmentDescription converted = {};
    converted.format = convert_pixel_format(attachmentDescription.format);
    converted.initialLayout = convert_layout(attachmentDescription.initialLayout);
    converted.finalLayout = convert_layout(attachmentDescription.finalLayout);
    converted.samples = VK_SAMPLE_COUNT_1_BIT;
    converted.loadOp = convert_load_op(attachmentDescription.loadOp);
    converted.storeOp = convert_store_op(attachmentDescription.storeOp);
    converted.storeOp = convert_store_op(attachmentDescription.storeOp);
    converted.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    converted.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    return converted;
}

VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassCreateInfo& vulkanRenderPassCreateInfo) :
    m_colorAttachments(vulkanRenderPassCreateInfo.colorAttachments, vulkanRenderPassCreateInfo.colorAttachments + vulkanRenderPassCreateInfo.colorAttachmentCount),
    m_device(vulkanRenderPassCreateInfo.device),
    m_hash(duk::hash::UndefinedHash) {

    if (vulkanRenderPassCreateInfo.depthAttachment) {
        m_depthAttachment = *vulkanRenderPassCreateInfo.depthAttachment;
    }

    std::vector<VkAttachmentDescription> attachments(m_colorAttachments.size());
    for (uint32_t i = 0; i < attachments.size(); i++) {
        attachments[i] = convert_attachment_description(m_colorAttachments[i]);
    }

    std::vector<VkAttachmentReference> colorAttachmentReferences(m_colorAttachments.size());

    for (uint32_t i = 0; i < colorAttachmentReferences.size(); i++) {
        colorAttachmentReferences[i].layout = convert_layout(m_colorAttachments[i].layout);
        colorAttachmentReferences[i].attachment = i;
    }

    VkAttachmentReference depthAttachmentReference = {};

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = colorAttachmentReferences.size();
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();

    if (m_depthAttachment.has_value()) {
        attachments.push_back(convert_attachment_description(m_depthAttachment.value()));

        depthAttachmentReference.layout = convert_layout(m_depthAttachment->layout);
        depthAttachmentReference.attachment = attachments.size() - 1;

        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    }

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    auto result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass);
    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to create a VkRenderPass");
    }

    m_clearValues.resize(attachments.size());
    for (int i = 0; i < m_clearValues.size(); i++) {
        if (i == m_clearValues.size() - 1) {
            m_clearValues[i].depthStencil = {1.0f, 0};
        }
        else {
            m_clearValues[i].color = {0.0f, 0.0f, 0.0f, 1.0f};
        }
    }

    m_hash = 0;
    duk::hash::hash_combine(m_hash, m_colorAttachments.begin(), m_colorAttachments.end());
    if (m_depthAttachment.has_value()) {
        duk::hash::hash_combine(m_hash, m_depthAttachment.value());
    }
}

VulkanRenderPass::~VulkanRenderPass() {
    clean();
}

void VulkanRenderPass::clean() {
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
}

void VulkanRenderPass::clean(uint32_t imageIndex) {
    // do nothing, it's only here so that we can avoid deleting it while at use at VulkanResourceManager
}

const std::vector<VkClearValue>& VulkanRenderPass::clear_values() const {
    return m_clearValues;
}

VkRenderPass VulkanRenderPass::handle() const {
    return m_renderPass;
}

size_t VulkanRenderPass::attachment_count() const {
    return color_attachment_count() + has_depth_attachment();
}

size_t VulkanRenderPass::color_attachment_count() const {
    return m_colorAttachments.size();
}

bool VulkanRenderPass::has_depth_attachment() const {
    return m_depthAttachment.has_value();
}

hash::Hash VulkanRenderPass::hash() const {
    return m_hash;
}


}