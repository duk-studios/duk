/// command_context.h

#ifndef DUK_RHI_COMMAND_CONTEXT_H
#define DUK_RHI_COMMAND_CONTEXT_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/image.h>
#include <duk_rhi/sampler.h>
#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/pipeline_flags.h>
#include <duk_rhi/pipeline_state.h>
#include <duk_rhi/shader.h>
#include <duk_rhi/shader_data_source.h>
#include <duk_rhi/pipeline_state_stack.h>

#include <array>

#include <duk_macros/macros.h>

#include <memory>

namespace duk::rhi {

class CommandQueue;

/// Shared, device-level context for resource creation, surface operations,
/// and command recording. A single CommandContext is associated with one
/// logical device and drives all GPU work
class CommandContext {
public:
    virtual ~CommandContext();

    //-------------------------------------------------------------------------
    // Pipeline state
    //-------------------------------------------------------------------------

    /// Returns the pipeline state stack for this context.
    /// Use push()/pop() to save/restore state; modify top() to set rendering state.
    PipelineStateStack& pipeline_state_stack();

    //-------------------------------------------------------------------------
    // Frame management
    //-------------------------------------------------------------------------

    /// Called once per frame, waits for the previous frame submission and resets
    /// state for a new fresh frame.
    virtual void update() = 0;

    /// Submits all recorded commands to the GPU, presents to screen if applicable.
    virtual void flush() = 0;

    //-------------------------------------------------------------------------
    // Render commands
    //-------------------------------------------------------------------------

    struct ShaderResources {
        std::array<Sampler, 16> samplers;
        std::array<const Image*, 16> sampledImages;
        std::array<const Image*, 16> storageImages;
        std::array<const Buffer*, 16> storageBuffers;
        std::array<const Buffer*, 16> uniformBuffers;
    };

    // used for both render and compute shaders
    struct BindShaderParams {
        const Shader* shader{nullptr};
        const ShaderResources* resources{nullptr};
    };

    struct RenderBeginParams {
        const FrameBuffer* frameBuffer{nullptr};
        LoadOp loadOp{LoadOp::LOAD};
        StoreOp storeOp{StoreOp::STORE};
        glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 1.0f};
    };

    virtual void render_begin(const RenderBeginParams& params) = 0;

    virtual void bind_render_shader(const BindShaderParams& params, const PipelineState& pipelineState) = 0;

    virtual void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) = 0;

    virtual void bind_index_buffer(const Buffer* indexBuffer) = 0;

    struct RenderParams {
        uint32_t vertexCount{0};
        uint32_t firstVertex{0};
        uint32_t instanceCount{1};
        uint32_t firstInstance{0};
    };

    virtual void render(const RenderParams& params) = 0;

    virtual void render_indirect(const std::span<const RenderParams>& indirectParams) = 0;

    struct RenderIndexedParams {
        uint32_t indexCount{0};
        uint32_t firstIndex{0};
        uint32_t instanceCount{1};
        uint32_t firstInstance{0};
        int32_t vertexOffset{0};
    };

    virtual void render_indexed(const RenderIndexedParams& params) = 0;

    virtual void render_indexed_indirect(const std::span<const RenderIndexedParams>& indexedIndirectParams) = 0;

    virtual void render_end() = 0;

    virtual void bind_compute_shader(const BindShaderParams& params) = 0;

    virtual void compute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    //-------------------------------------------------------------------------
    // Resource creation
    //-------------------------------------------------------------------------

    struct ShaderCreateInfo {
        const ShaderDataSource* shaderDataSource;
    };

    DUK_NO_DISCARD virtual std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) = 0;

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

    /// Creates an empty FrameBuffer object. No VkFramebuffer is allocated until
    /// write_frame_buffer() is called.
    DUK_NO_DISCARD virtual std::shared_ptr<FrameBuffer> create_frame_buffer() = 0;

    /// Builds or rebuilds the underlying GPU framebuffer for the given attachments.
    /// If the framebuffer was previously written its old GPU handle is deferred for
    /// destruction until the current frame's GPU fence is signalled.
    virtual void write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) = 0;

private:
    PipelineStateStack m_pipelineStateStack;
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_CONTEXT_H

