/// command_context.h

#ifndef DUK_RHI_COMMAND_CONTEXT_H
#define DUK_RHI_COMMAND_CONTEXT_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/descriptor_set.h>
#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/image.h>
#include <duk_rhi/image_data_source.h>
#include <duk_rhi/pipeline/compute_pipeline.h>
#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/pipeline/pipeline_flags.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/pipeline/shader_data_source.h>
#include <duk_rhi/render_pass.h>

#include <duk_macros/macros.h>

#include <memory>

namespace duk::rhi {

class CommandQueue;

/// Shared, device-level context for resource creation, surface operations,
/// and command recording. A single CommandContext is associated with one
/// logical device and drives all GPU work
class CommandContext {
public:
    virtual ~CommandContext() = default;

    //-------------------------------------------------------------------------
    // Frame management
    //-------------------------------------------------------------------------

    /// Called once per frame, waits for the previous frame submission and resets
    /// state for a new fresh frame
    virtual void update() = 0;

    /// Submits all recorded commands to the GPU, presents to screen if applicable
    virtual void flush() = 0;

    //-------------------------------------------------------------------------
    // Command recording
    //-------------------------------------------------------------------------

    virtual void begin_render_pass(RenderPass* renderPass, FrameBuffer* frameBuffer) = 0;

    virtual void end_render_pass() = 0;

    virtual void bind_graphics_pipeline(GraphicsPipeline* pipeline) = 0;

    virtual void bind_compute_pipeline(ComputePipeline* pipeline) = 0;

    virtual void bind_vertex_buffer(const Buffer** buffers, uint32_t bufferCount, uint32_t firstBinding) = 0;

    virtual void bind_index_buffer(const Buffer* buffer) = 0;

    virtual void bind_descriptor_set(DescriptorSet* descriptorSet, uint32_t setIndex) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

    virtual void draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;

    virtual void draw_indirect(const Buffer* buffer, size_t offset, uint32_t drawCount) = 0;

    virtual void draw_indirect_indexed(const Buffer* buffer, size_t offset, uint32_t drawCount) = 0;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    struct BufferMemoryBarrier {
        PipelineStage::Mask srcStageMask;
        Access::Mask srcAccessMask;
        PipelineStage::Mask dstStageMask;
        Access::Mask dstAccessMask;
        CommandQueue* srcCommandQueue;
        CommandQueue* dstCommandQueue;
        Buffer* buffer;
        size_t offset;
        size_t size;
    };

    struct ImageMemoryBarrier {
        PipelineStage::Mask srcStageMask;
        Access::Mask srcAccessMask;
        PipelineStage::Mask dstStageMask;
        Access::Mask dstAccessMask;
        CommandQueue* srcCommandQueue;
        CommandQueue* dstCommandQueue;
        Image* image;
        Image::SubresourceRange subresourceRange;
    };

    struct PipelineBarrier {
        uint32_t bufferMemoryBarrierCount;
        const BufferMemoryBarrier* bufferMemoryBarriers;
        uint32_t imageMemoryBarrierCount;
        const ImageMemoryBarrier* imageMemoryBarriers;
    };

    virtual void pipeline_barrier(const PipelineBarrier& barrier) = 0;

    //-------------------------------------------------------------------------
    // Resource creation
    //-------------------------------------------------------------------------

    struct ShaderCreateInfo {
        const ShaderDataSource* shaderDataSource;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) = 0;

    struct GraphicsPipelineCreateInfo {
        Shader* shader;
        RenderPass* renderPass;
        GraphicsPipeline::Viewport viewport;
        GraphicsPipeline::Scissor scissor;
        GraphicsPipeline::CullMode::Mask cullModeMask;
        GraphicsPipeline::Blend blend;
        GraphicsPipeline::Topology topology;
        GraphicsPipeline::FillMode fillMode;
        bool depthTesting;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<GraphicsPipeline> create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) = 0;

    struct ComputePipelineCreateInfo {
        Shader* shader;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<ComputePipeline> create_compute_pipeline(const ComputePipelineCreateInfo& pipelineCreateInfo) = 0;

    struct RenderPassCreateInfo {
        AttachmentDescription* colorAttachments;
        uint32_t colorAttachmentCount;
        AttachmentDescription* depthAttachment;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<RenderPass> create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) = 0;

    struct BufferCreateInfo {
        Buffer::Type type;
        Buffer::UpdateFrequency updateFrequency;
        size_t size;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<Buffer> create_buffer(const BufferCreateInfo& bufferCreateInfo) = 0;

    /// Write CPU data into a buffer.
    /// For DYNAMIC buffers: maps host-visible memory and copies directly.
    /// For STATIC buffers: records a staging upload into the current command buffer.
    virtual void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) = 0;

    /// Read buffer data back to CPU.
    /// For DYNAMIC buffers only: maps host-visible memory and copies directly.
    virtual void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) = 0;

    struct ImageCreateInfo {
        PixelFormat format;
        uint32_t width;
        uint32_t height;
        Image::Usage usage;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<Image> create_image(const ImageCreateInfo& imageCreateInfo) = 0;

    /// Upload raw pixel data into an image, recording a staging copy into the current command buffer.
    virtual void write_image(Image* image, const void* src, size_t size) = 0;

    /// Convenience overload — reads all bytes from the data source and delegates to the raw overload.
    virtual void write_image(Image* image, const ImageDataSource* dataSource) = 0;

    struct DescriptorSetCreateInfo {
        DescriptorSetDescription description;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<DescriptorSet> create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) = 0;

    struct FrameBufferCreateInfo {
        RenderPass* renderPass;
        Image** attachments;
        uint32_t attachmentCount;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<FrameBuffer> create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_CONTEXT_H

