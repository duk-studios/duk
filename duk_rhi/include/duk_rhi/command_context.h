/// command_context.h

#ifndef DUK_RHI_COMMAND_CONTEXT_H
#define DUK_RHI_COMMAND_CONTEXT_H

#include <duk_rhi/buffer.h>
#include <duk_rhi/image.h>
#include <duk_rhi/sampler.h>
#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/pipeline_state.h>
#include <duk_rhi/shader.h>
#include <duk_rhi/shader_data_source.h>
#include <duk_rhi/pipeline_state_stack.h>

#include <array>

#include <duk_macros/macros.h>

#include <memory>

namespace duk::rhi {

class CommandQueue;
class CommandContext;


struct ShaderCreateInfo {
    const ShaderDataSource* shaderDataSource;
};

struct BufferCreateInfo {
    Buffer::Type type;
    Buffer::UpdateFrequency updateFrequency;
    size_t size;
};

struct ImageCreateInfo {
    PixelFormat format;
    uint32_t width;
    uint32_t height;
    Image::Usage usage;
};

struct ShaderResources {
    std::array<Sampler, 16> samplers;
    std::array<const Image*, 16> sampledImages;
    std::array<const Image*, 16> storageImages;
    std::array<const Buffer*, 16> storageBuffers;
    std::array<const Buffer*, 16> uniformBuffers;
};

struct BindShaderParams {
    const Shader* shader{nullptr};
    const ShaderResources* resources{nullptr};
};

struct RenderBeginParams {
    const FrameBuffer* frameBuffer{nullptr};
    LoadOp loadOp{LoadOp::CLEAR};
    StoreOp storeOp{StoreOp::STORE};
    glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 1.0f};
};

struct RenderParams {
    uint32_t vertexCount{0};
    uint32_t firstVertex{0};
    uint32_t instanceCount{1};
    uint32_t firstInstance{0};
};

struct RenderIndexedParams {
    uint32_t indexCount{0};
    uint32_t firstIndex{0};
    uint32_t instanceCount{1};
    uint32_t firstInstance{0};
    int32_t vertexOffset{0};
};

class ScopedCommands {
public:
    bool valid() const;
protected:
    explicit ScopedCommands(CommandContext& context);
    ScopedCommands(const ScopedCommands&) = delete;
    ScopedCommands& operator=(const ScopedCommands&) = delete;
    ScopedCommands(ScopedCommands&&) = delete;
    ScopedCommands& operator=(ScopedCommands&&) = delete;

    CommandContext& m_context;
    uint64_t m_commandId;
};

class RenderCommands : public ScopedCommands {
public:
    explicit RenderCommands(CommandContext& context, const RenderBeginParams& params);
    ~RenderCommands();

    void bind_shader(const BindShaderParams& params, const PipelineState& pipelineState) const;

    void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) const;

    void bind_index_buffer(const Buffer* indexBuffer) const;

    void render(const RenderParams& params) const;

    void render_indirect(const std::span<const RenderParams>& indirectParams) const;

    void render_indexed(const RenderIndexedParams& params) const;

    void render_indexed_indirect(const std::span<const RenderIndexedParams>& indexedIndirectParams) const;
};

class ComputeCommands : public ScopedCommands {
public:
    explicit ComputeCommands(CommandContext& context);

    void bind_shader(const BindShaderParams& params) const;

    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const;
};

class TransferCommands : public ScopedCommands {
public:
    explicit TransferCommands(CommandContext& context);

    void write_image(Image* image, const void* src, size_t size) const;

    void write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) const;

    void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) const;

    void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) const;
};

/// Shared, device-level context for resource creation, surface operations,
/// and command recording. A single CommandContext is associated with one
/// logical device and drives all GPU work
class CommandContext {
public:
    virtual ~CommandContext();

    //-------------------------------------------------------------------------
    // Scoped command objects
    //-------------------------------------------------------------------------

    RenderCommands render(const RenderBeginParams& params);

    ComputeCommands compute();

    TransferCommands transfer();

    //-------------------------------------------------------------------------
    // Frame management
    //-------------------------------------------------------------------------

    /// enables the recording of commands
    virtual void prepare() = 0;

    /// Submits all recorded commands to the GPU, presents to screen if applicable.
    virtual void flush() = 0;

    //-------------------------------------------------------------------------
    // Resource creation
    //-------------------------------------------------------------------------

    DUK_NO_DISCARD virtual std::shared_ptr<Shader> create_shader(const ShaderCreateInfo& shaderCreateInfo) = 0;

    DUK_NO_DISCARD virtual std::shared_ptr<Buffer> create_buffer(const BufferCreateInfo& bufferCreateInfo) = 0;

    DUK_NO_DISCARD virtual std::shared_ptr<Image> create_image(const ImageCreateInfo& imageCreateInfo) = 0;

    DUK_NO_DISCARD virtual std::shared_ptr<FrameBuffer> create_frame_buffer() = 0;

    //-------------------------------------------------------------------------
    // Render commands
    //-------------------------------------------------------------------------

    virtual void render_begin(const RenderBeginParams& params) = 0;

    virtual void bind_render_shader(const BindShaderParams& params, const PipelineState& pipelineState) = 0;

    virtual void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) = 0;

    virtual void bind_index_buffer(const Buffer* indexBuffer) = 0;

    virtual void render(const RenderParams& params) = 0;

    virtual void render_indirect(const std::span<const RenderParams>& indirectParams) = 0;

    virtual void render_indexed(const RenderIndexedParams& params) = 0;

    virtual void render_indexed_indirect(const std::span<const RenderIndexedParams>& indexedIndirectParams) = 0;

    virtual void render_end() = 0;

    //-------------------------------------------------------------------------
    // Compute commands
    //-------------------------------------------------------------------------

    virtual void bind_compute_shader(const BindShaderParams& params) = 0;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    //-------------------------------------------------------------------------
    // Transfer commands
    //-------------------------------------------------------------------------

    virtual void write_image(Image* image, const void* src, size_t size) = 0;

    virtual void write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) = 0;

    virtual void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) = 0;

    virtual void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) = 0;

private:
    friend ScopedCommands;
    uint64_t m_commandCounter{0};
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_CONTEXT_H

