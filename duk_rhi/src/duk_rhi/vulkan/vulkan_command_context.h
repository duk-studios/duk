/// vulkan_command_context.h

#ifndef DUK_RHI_VULKAN_COMMAND_CONTEXT_H
#define DUK_RHI_VULKAN_COMMAND_CONTEXT_H

#include <duk_rhi/command/command_context.h>
#include <duk_rhi/vulkan/vulkan_deletion_queue.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_queue.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>

#include <memory>
#include <vector>


namespace duk::rhi {

class VulkanDescriptorSetLayoutCache;

struct VulkanCommandContextCreateInfo {
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    VulkanQueue* queue;
    uint32_t framesInFlight;
};

class VulkanCommandContext : public CommandContext {
public:
    explicit VulkanCommandContext(const VulkanCommandContextCreateInfo& createInfo, std::unique_ptr<VulkanSwapchain> swapchain);

    ~VulkanCommandContext() override;

    // -----------------------------------------------------------------------
    // Frame management
    // -----------------------------------------------------------------------
    void update() override;
    void flush() override;


    // -----------------------------------------------------------------------
    // Command recording (CommandContext interface)
    // -----------------------------------------------------------------------
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

    // -----------------------------------------------------------------------
    // Resource creation (CommandContext interface)
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<GraphicsPipeline> create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<ComputePipeline> create_compute_pipeline(const ComputePipelineCreateInfo& pipelineCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<RenderPass> create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<Buffer> create_buffer(const BufferCreateInfo& bufferCreateInfo) override;
    void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) override;
    void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) override;
    DUK_NO_DISCARD std::shared_ptr<Image> create_image(const ImageCreateInfo& imageCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<DescriptorSet> create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<FrameBuffer> create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) override;

    // -----------------------------------------------------------------------
    // Internal accessors
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD uint32_t current_frame() const;
    DUK_NO_DISCARD const uint32_t* current_frame_ptr() const;
    DUK_NO_DISCARD uint32_t current_image() const;
    DUK_NO_DISCARD const uint32_t* current_image_ptr() const;

    DUK_NO_DISCARD VkDevice device() const;
    DUK_NO_DISCARD VulkanPhysicalDevice* physical_device() const;
    DUK_NO_DISCARD VulkanDescriptorSetLayoutCache* descriptor_set_layout_cache() const;
    DUK_NO_DISCARD VulkanSamplerCache* sampler_cache() const;

    /// Returns the current value of the timeline semaphore counter.
    DUK_NO_DISCARD VkSemaphore timeline_semaphore() const;
    DUK_NO_DISCARD uint64_t timeline_value() const;

private:
    /// Lazily acquires and begins the per-frame command buffer on first recording call.
    DUK_NO_DISCARD VkCommandBuffer current_command_buffer();

    /// Wraps a heap-allocated resource in a shared_ptr whose custom deleter pushes
    /// the raw pointer into the deletion queue instead of destroying it immediately.
    /// This guarantees the object is only freed after the next fence wait.
    ///
    /// The deletion queue is captured as a shared_ptr so it safely outlives both
    /// the context and any lingering shared_ptr<T> the caller may hold.
    template<typename T>
    DUK_NO_DISCARD std::shared_ptr<T> make_managed(T* rawPtr) {
        auto queue = m_deletionQueue;
        return std::shared_ptr<T>(rawPtr, [queue](T* ptr) {
            queue->push(ptr);
        });
    }

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    VulkanQueue* m_queue;
    uint32_t m_framesInFlight;
    uint32_t m_currentFrame{0};
    uint32_t m_currentImage{0};

    // Per-frame CPU-GPU fences (owned here, not in the swapchain)
    std::vector<VkFence> m_fences;

    // Single timeline semaphore incremented on every flush()
    VkSemaphore m_timelineSemaphore{VK_NULL_HANDLE};
    uint64_t m_timelineValue{0};

    // Binary swapchain semaphores for the current frame (borrowed from VulkanSwapchain)
    VkSemaphore m_imageAvailableSemaphore{VK_NULL_HANDLE};
    VkSemaphore m_renderFinishedSemaphore{VK_NULL_HANDLE};
    bool m_swapchainImageAcquired{false};

    // Lazy command buffer cache
    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkCommandBuffer m_activeCommandBuffer{VK_NULL_HANDLE};

    // Current pipeline recording state
    VkPipelineLayout m_currentPipelineLayout{VK_NULL_HANDLE};
    VkPipelineBindPoint m_currentPipelineBindPoint{VK_PIPELINE_BIND_POINT_GRAPHICS};

    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanDescriptorSetLayoutCache> m_descriptorSetLayoutCache;
    std::unique_ptr<VulkanSamplerCache> m_samplerCache;

    /// Deferred-deletion queue — destroyed after each frame's fence wait so the
    /// GPU is guaranteed to have finished using the resources before they vanish.
    /// Stored as shared_ptr so custom deleters can capture it safely.
    std::shared_ptr<VulkanDeletionQueue> m_deletionQueue;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_COMMAND_CONTEXT_H

