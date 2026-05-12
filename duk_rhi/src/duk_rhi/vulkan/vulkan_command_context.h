/// vulkan_command_context.h

#ifndef DUK_RHI_VULKAN_COMMAND_CONTEXT_H
#define DUK_RHI_VULKAN_COMMAND_CONTEXT_H

#include <duk_rhi/command_context.h>
#include <duk_rhi/vulkan/vulkan_deletion_queue.h>
#include <duk_rhi/vulkan/vulkan_import.h>
#include <duk_rhi/vulkan/vulkan_physical_device.h>
#include <duk_rhi/vulkan/vulkan_queue.h>
#include <duk_rhi/vulkan/vulkan_sampler.h>
#include <duk_rhi/vulkan/vulkan_swapchain.h>
#include <duk_rhi/vulkan/vulkan_render_state.h>

#include <memory>
#include <vector>


namespace duk::rhi {

class VulkanDescriptorSetLayoutCache;

struct VulkanCommandContextCreateInfo {
    VulkanInstance* instance;
    const VulkanPhysicalDevice* physicalDevice;
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
    void prepare() override;
    void flush() override;

    // -----------------------------------------------------------------------
    // Resource creation (CommandContext interface)
    // -----------------------------------------------------------------------
    DUK_NO_DISCARD std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<Buffer> create_buffer(const BufferCreateInfo& bufferCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<Image> create_image(const ImageCreateInfo& imageCreateInfo) override;
    DUK_NO_DISCARD std::shared_ptr<FrameBuffer> create_frame_buffer() override;

    // -----------------------------------------------------------------------
    // Command recording (CommandContext interface)
    // -----------------------------------------------------------------------
    void render_begin(const RenderBeginParams& params) override;
    void bind_render_shader(const BindShaderParams& params, const PipelineState& pipelineState) override;
    void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) override;
    void bind_index_buffer(const Buffer* indexBuffer) override;
    void render(const RenderParams& params) override;
    void render_indirect(const std::span<const RenderParams>& indirectParams) override;
    void render_indexed(const RenderIndexedParams& params) override;
    void render_indexed_indirect(const std::span<const RenderIndexedParams>& indexedIndirectParams) override;
    void render_end() override;
    void bind_compute_shader(const BindShaderParams& params) override;
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    void write_image(Image* image, const void* src, size_t size) override;
    void write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) override;
    void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) override;
    void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) override;

private:

    template<typename T, typename ...Args>
    DUK_NO_DISCARD std::shared_ptr<T> make_managed(Args&&... args) {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...), [this](T* ptr) {
            m_deletionQueue.push(ptr, m_frameCounter);
        });
    }

private:
    VulkanInstance& m_instance;
    VkDevice m_device;
    const VulkanPhysicalDevice& m_physicalDevice;
    VulkanQueue* m_queue;
    uint32_t m_framesInFlight;
    uint32_t m_frameCounter{0};
    uint32_t m_frameIndex{0};
    uint32_t m_swapchainImageIndex{0};

    // Sync
    std::vector<VkFence> m_fences;
    std::vector<VkSemaphore> m_imageSemaphores;
    std::vector<VkSemaphore> m_renderSemaphores;

    // Lazy command buffer cache
    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkCommandBuffer m_activeCommandBuffer{VK_NULL_HANDLE};

    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanDescriptorSetLayoutCache> m_descriptorSetLayoutCache;
    std::unique_ptr<VulkanSamplerCache> m_samplerCache;
    std::unique_ptr<VulkanRenderState> m_renderState;
    std::unique_ptr<VulkanFrameBuffer> m_defaultFrameBuffer;

    VulkanDeletionQueue m_deletionQueue;
};

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_COMMAND_CONTEXT_H

