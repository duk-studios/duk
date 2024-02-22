/// 23/04/2023
/// vulkan_render_pass.h

#ifndef DUK_RHI_VULKAN_RENDER_PASS_H
#define DUK_RHI_VULKAN_RENDER_PASS_H

#include <duk_rhi/render_pass.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <memory>
#include <optional>
#include <unordered_map>

namespace duk::rhi {

VkAttachmentStoreOp convert_store_op(StoreOp storeOp);

VkAttachmentLoadOp convert_load_op(LoadOp loadOp);

struct VulkanRenderPassCreateInfo {
    AttachmentDescription* colorAttachments;
    uint32_t colorAttachmentCount;
    AttachmentDescription* depthAttachment;
    VkDevice device;
};

class VulkanRenderPass : public RenderPass {
public:
    explicit VulkanRenderPass(const VulkanRenderPassCreateInfo& vulkanRenderPassCreateInfo);

    ~VulkanRenderPass() override;

    void clean();

    void clean(uint32_t imageIndex);

    DUK_NO_DISCARD const std::vector<VkClearValue>& clear_values() const;

    DUK_NO_DISCARD VkRenderPass handle() const;

    DUK_NO_DISCARD size_t attachment_count() const override;

    DUK_NO_DISCARD size_t color_attachment_count() const override;

    DUK_NO_DISCARD bool has_depth_attachment() const override;

    DUK_NO_DISCARD duk::hash::Hash hash() const override;

private:
    std::vector<AttachmentDescription> m_colorAttachments;
    std::optional<AttachmentDescription> m_depthAttachment;
    VkDevice m_device;
    VkRenderPass m_renderPass;
    std::vector<VkClearValue> m_clearValues;
    duk::hash::Hash m_hash;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_RENDER_PASS_H
