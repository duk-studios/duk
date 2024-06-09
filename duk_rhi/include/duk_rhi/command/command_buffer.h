/// 21/04/2023
/// command_interface.h

#ifndef DUK_RHI_COMMAND_INTERFACE_H
#define DUK_RHI_COMMAND_INTERFACE_H

#include <duk_rhi/command/command.h>
#include <duk_rhi/image.h>
#include <duk_rhi/pipeline/pipeline_flags.h>

namespace duk::rhi {

class CommandQueue;
class RenderPass;
class Buffer;
class FrameBuffer;
class GraphicsPipeline;
class ComputePipeline;
class DescriptorSet;

class CommandBuffer : public Command {
public:
    ~CommandBuffer() override = default;

    virtual void begin() = 0;

    virtual void end() = 0;

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
        Image::Layout oldLayout;
        Image::Layout newLayout;
        Image::SubresourceRange subresourceRange;
    };

    struct PipelineBarrier {
        uint32_t bufferMemoryBarrierCount;
        const BufferMemoryBarrier* bufferMemoryBarriers;
        uint32_t imageMemoryBarrierCount;
        const ImageMemoryBarrier* imageMemoryBarriers;
    };

    virtual void pipeline_barrier(const PipelineBarrier& barrier) = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_INTERFACE_H
