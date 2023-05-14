/// 21/04/2023
/// vulkan_command_interface.h

#ifndef DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H
#define DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

#include <duk_renderer/command_buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_command.h>

namespace duk::renderer {

class VulkanCommandQueue;
class VulkanRenderer;

struct VulkanCommandBufferCreateInfo {
    VulkanCommandQueue* commandQueue;
    uint32_t frameCount;
    uint32_t* currentFramePtr;
};

class VulkanCommandBuffer : public CommandBuffer, public VulkanCommand {
public:
    explicit VulkanCommandBuffer(const VulkanCommandBufferCreateInfo& commandInterfaceCreateInfo);

    ~VulkanCommandBuffer() override;

    void begin() override;

    void end() override;

    DUK_NO_DISCARD VulkanCommandQueue* command_queue();

    DUK_NO_DISCARD VkCommandBuffer& handle();

    // CommandBuffer overrides
    void begin_render_pass(const RenderPassBeginInfo& renderPassBeginInfo) override;

    void end_render_pass() override;

    void render(const RenderMeshInfo& renderMeshInfo) override;

    // VulkanCommand overrides
    void submit(const VulkanCommandParams& params) override;

private:
    VulkanCommandQueue* m_commandQueue;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkCommandBuffer m_currentCommandBuffer;
    uint32_t* m_currentFramePtr;
};

}

#endif // DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

