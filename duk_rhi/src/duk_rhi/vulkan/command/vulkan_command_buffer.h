/// 21/04/2023
/// vulkan_command_interface.h

#ifndef DUK_RHI_VULKAN_COMMAND_INTERFACE_H
#define DUK_RHI_VULKAN_COMMAND_INTERFACE_H

#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/vulkan/command/vulkan_command.h>
#include <duk_rhi/vulkan/vulkan_import.h>

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

    void clean(uint32_t imageIndex);

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

    void bind_vertex_buffer(const Buffer** buffers, uint32_t bufferCount, uint32_t firstBinding) override;

    void bind_index_buffer(const Buffer* buffer) override;

    void bind_descriptor_set(DescriptorSet* descriptorSet, uint32_t setIndex) override;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

    void draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

    void draw_indirect(const Buffer* buffer, size_t offset, uint32_t drawCount) override;

    void draw_indirect_indexed(const Buffer* buffer, size_t offset, uint32_t drawCount) override;

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

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_COMMAND_INTERFACE_H
