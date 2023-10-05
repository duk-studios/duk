/// 21/04/2023
/// vulkan_command_interface.h

#ifndef DUK_RHI_VULKAN_COMMAND_INTERFACE_H
#define DUK_RHI_VULKAN_COMMAND_INTERFACE_H

#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/command/vulkan_command.h>

namespace duk::rhi {

class VulkanCommandQueue;
class VulkanRHI;
class VulkanGraphicsPipeline;
class VulkanComputePipeline;

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

    void begin_render_pass(RenderPass* renderPass, FrameBuffer* frameBuffer) override;

    void end_render_pass() override;

    void bind_graphics_pipeline(GraphicsPipeline* pipeline) override;

    void bind_compute_pipeline(ComputePipeline* pipeline) override;

    void bind_vertex_buffer(Buffer* buffer) override;

    void bind_index_buffer(Buffer* buffer) override;

    void bind_descriptor_set(DescriptorSet* descriptorSet, uint32_t setIndex) override;

    void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) override;

    void draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    void pipeline_barrier(const PipelineBarrier& barrier) override;

    // Command overrides
    DUK_NO_DISCARD Submitter* submitter_ptr() override;

private:
    VulkanCommandQueue* m_commandQueue;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkCommandBuffer m_currentCommandBuffer;
    const uint32_t* m_currentImagePtr;
    VulkanSubmitter m_submitter;

    // state cache
    VkPipelineLayout m_currentPipelineLayout;
    VkPipelineBindPoint m_currentPipelineBindPoint;

};

}

#endif // DUK_RHI_VULKAN_COMMAND_INTERFACE_H

