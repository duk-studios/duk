/// 23/04/2023
/// vulkan_render_pass.cpp

#include <duk_renderer/vulkan/vulkan_render_pass.h>
#include <duk_renderer/vulkan/vulkan_image.h>

namespace duk::renderer {

namespace vk {

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
    converted.format = vk::convert_pixel_format(attachmentDescription.format);
    converted.initialLayout = vk::convert_layout(attachmentDescription.initialLayout);
    converted.finalLayout = vk::convert_layout(attachmentDescription.finalLayout);
    converted.samples = VK_SAMPLE_COUNT_1_BIT;
    converted.loadOp = vk::convert_load_op(attachmentDescription.loadOp);
    converted.storeOp = vk::convert_store_op(attachmentDescription.storeOp);
    converted.storeOp = vk::convert_store_op(attachmentDescription.storeOp);
    converted.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    converted.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    return converted;
}

}

VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassCreateInfo& vulkanRenderPassCreateInfo) :
    m_colorAttachments(vulkanRenderPassCreateInfo.colorAttachments, vulkanRenderPassCreateInfo.colorAttachments + vulkanRenderPassCreateInfo.colorAttachmentCount),
    m_device(vulkanRenderPassCreateInfo.device) {

    if (vulkanRenderPassCreateInfo.depthAttachment) {
        m_depthAttachment = *vulkanRenderPassCreateInfo.depthAttachment;
    }

    std::vector<VkAttachmentDescription> attachments(m_colorAttachments.size());
    for (uint32_t i = 0; i < attachments.size(); i++) {
        attachments[i] = vk::convert_attachment_description(m_colorAttachments[i]);
    }

    std::vector<VkAttachmentReference> colorAttachmentReferences(m_colorAttachments.size());

    for (uint32_t i = 0; i < colorAttachmentReferences.size(); i++) {
        colorAttachmentReferences[i].layout = vk::convert_layout(m_colorAttachments[i].layout);
        colorAttachmentReferences[i].attachment = i;
    }

    VkAttachmentReference depthAttachmentReference = {};

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = colorAttachmentReferences.size();
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();

    if (m_depthAttachment.has_value()) {
        attachments.push_back(vk::convert_attachment_description(m_depthAttachment.value()));

        depthAttachmentReference.layout = vk::convert_layout(m_depthAttachment->layout);
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
}

VulkanRenderPass::~VulkanRenderPass() {
    clean();
}

VkRenderPass duk::renderer::VulkanRenderPass::handle() const {
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

void VulkanRenderPass::clean() {
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
}

void VulkanRenderPass::clean(uint32_t imageIndex) {
    // do nothing, it's only here so that we can avoid deleting it while at use at VulkanResourceManager
}

}