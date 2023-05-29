/// 21/04/2023
/// vulkan_command_interface.h

#ifndef DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H
#define DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

#include <duk_renderer/command/command_buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/command/vulkan_command.h>

namespace duk::renderer {

class VulkanCommandQueue;
class VulkanRenderer;

struct VulkanCommandBufferCreateInfo {
    VkDevice device;
    VulkanCommandQueue* commandQueue;
    uint32_t frameCount;
    const uint32_t* currentFramePtr;
    const uint32_t* currentImagePtr;
};

class VulkanCommandBuffer : public CommandBuffer {
public:
    explicit VulkanCommandBuffer(const VulkanCommandBufferCreateInfo& commandBufferCreateInfo);

    ~VulkanCommandBuffer() override;

    DUK_NO_DISCARD VulkanCommandQueue* command_queue();

    DUK_NO_DISCARD VkCommandBuffer& handle();

    void submit(const VulkanCommandParams& params);

    // CommandBuffer overrides
    void begin() override;

    void end() override;

    void begin_render_pass(const RenderPassBeginInfo& renderPassBeginInfo) override;

    void end_render_pass() override;

    void bind_pipeline(Pipeline* pipeline) override;

    void bind_vertex_buffer(Buffer* buffer) override;

    void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) override;

    // Command overrides
    DUK_NO_DISCARD Submitter* submitter_ptr() override;

private:
    VulkanCommandQueue* m_commandQueue;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkCommandBuffer m_currentCommandBuffer;
    const uint32_t* m_currentImagePtr;
    VulkanSubmitter m_submitter;
};

}

#endif // DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

