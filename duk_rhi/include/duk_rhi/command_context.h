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

#include <array>
#include <variant>

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

/// Binds a sampled image (texture + sampler pair) to a logical descriptor slot.
struct ImageBinding {
    const Image* image{nullptr};
    Sampler sampler{};
};

/// Binds a buffer (uniform or storage) to a logical descriptor slot.
struct BufferBinding {
    const Buffer* buffer{nullptr};
    uint32_t offset{0};
};

/// A single resource bound to one logical descriptor slot.
/// std::monostate means the slot is unused.
using ShaderResource = std::variant<std::monostate, ImageBinding, BufferBinding>;

static constexpr uint32_t kMaxShaderBindings = 32;

/// Flat array of resources indexed by logical binding slot.
/// Each slot index corresponds to the generated enum value for the named descriptor.
struct ShaderResources {
    std::array<ShaderResource, kMaxShaderBindings> bindings{};
};

struct RenderBeginParams {
    const FrameBuffer* frameBuffer{nullptr};
    LoadOp loadOp{LoadOp::CLEAR};
    StoreOp storeOp{StoreOp::STORE};
    glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 1.0f};
};

struct DrawParams {
    uint32_t vertexCount{0};
    uint32_t firstVertex{0};
    uint32_t instanceCount{1};
    uint32_t firstInstance{0};
};

struct DrawIndexedParams {
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

    void bind_shader(const Shader* shader, const PipelineState& pipelineState) const;

    void bind_resources(const ShaderResources& resources) const;

    void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) const;

    void bind_index_buffer(const Buffer* indexBuffer) const;

    void draw(const DrawParams& params) const;

    void draw_indirect(const std::span<const DrawParams>& indirectParams) const;

    void draw_indexed(const DrawIndexedParams& params) const;

    void draw_indexed_indirect(const std::span<const DrawIndexedParams>& indexedIndirectParams) const;
};

class ComputeCommands : public ScopedCommands {
public:
    explicit ComputeCommands(CommandContext& context);

    void bind_shader(const Shader* shader) const;

    void bind_resources(const ShaderResources& resources) const;

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

    /// Enables the recording of commands
    virtual void prepare() = 0;

    /// Enables the recording of commands and prepares an image for presentation.
    /// The next submit call will also present to the attached window (requires a window in this context)
    virtual void prepare_present() = 0;

    /// Submits all recorded commands to the GPU, presents to screen if applicable.
    virtual void submit() = 0;

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

    virtual void bind_render_shader(const Shader* shader, const PipelineState& pipelineState) = 0;

    virtual void bind_resources(const ShaderResources& resources) = 0;

    virtual void bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) = 0;

    virtual void bind_index_buffer(const Buffer* indexBuffer) = 0;

    virtual void draw(const DrawParams& params) = 0;

    virtual void draw_indirect(const std::span<const DrawParams>& indirectParams) = 0;

    virtual void draw_indexed(const DrawIndexedParams& params) = 0;

    virtual void draw_indexed_indirect(const std::span<const DrawIndexedParams>& indexedIndirectParams) = 0;

    virtual void render_end() = 0;

    //-------------------------------------------------------------------------
    // Compute commands
    //-------------------------------------------------------------------------

    virtual void bind_compute_shader(const Shader* shader) = 0;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    //-------------------------------------------------------------------------
    // Transfer commands
    //-------------------------------------------------------------------------

    virtual void write_image(Image* image, const void* src, size_t size) = 0;

    virtual void write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) = 0;

    virtual void write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) = 0;

    virtual void read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) = 0;

    virtual void map_buffer(Buffer* buffer) = 0;

    virtual void unmap_buffer(Buffer* buffer) = 0;

    virtual void flush_buffer(Buffer* buffer, size_t offset, size_t size) = 0;

private:
    friend ScopedCommands;
    uint64_t m_commandCounter{0};
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_CONTEXT_H
