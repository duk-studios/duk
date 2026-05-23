/// command_context.cpp

#include <duk_rhi/command_context.h>

namespace duk::rhi {

bool ScopedCommands::valid() const {
    return m_commandId == m_context.m_commandCounter;
}

ScopedCommands::ScopedCommands(CommandContext& context)
    : m_context(context)
    , m_commandId(++m_context.m_commandCounter) {
}

RenderCommands::RenderCommands(CommandContext& context, const RenderBeginParams& params)
    : ScopedCommands(context) {
    m_context.render_begin(params);
}

RenderCommands::~RenderCommands() {
    DUK_ASSERT(valid());
    m_context.render_end();
}

void RenderCommands::bind_shader(const Shader* shader, const PipelineState& pipelineState) const {
    DUK_ASSERT(valid());
    m_context.bind_render_shader(shader, pipelineState);
}

void RenderCommands::bind_resources(const ShaderResources& resources) const {
    DUK_ASSERT(valid());
    m_context.bind_resources(resources);
}

void RenderCommands::bind_vertex_buffers(const Buffer* const* vertexBuffers, uint32_t count) const {
    DUK_ASSERT(valid());
    m_context.bind_vertex_buffers(vertexBuffers, count);
}

void RenderCommands::bind_index_buffer(const Buffer* indexBuffer) const {
    DUK_ASSERT(valid());
    m_context.bind_index_buffer(indexBuffer);
}

void RenderCommands::draw(const DrawParams& params) const {
    DUK_ASSERT(valid());
    m_context.draw(params);
}

void RenderCommands::draw_indirect(const std::span<const DrawParams>& indirectParams) const {
    DUK_ASSERT(valid());
    m_context.draw_indirect(indirectParams);
}

void RenderCommands::draw_indexed(const DrawIndexedParams& params) const {
    DUK_ASSERT(valid());
    m_context.draw_indexed(params);
}

void RenderCommands::draw_indexed_indirect(const std::span<const DrawIndexedParams>& indexedIndirectParams) const {
    DUK_ASSERT(valid());
    m_context.draw_indexed_indirect(indexedIndirectParams);
}

ComputeCommands::ComputeCommands(CommandContext& context)
    : ScopedCommands(context) {
}

void ComputeCommands::bind_shader(const Shader* shader) const {
    DUK_ASSERT(valid());
    m_context.bind_compute_shader(shader);
}

void ComputeCommands::bind_resources(const ShaderResources& resources) const {
    DUK_ASSERT(valid());
    m_context.bind_resources(resources);
}

void ComputeCommands::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const {
    DUK_ASSERT(valid());
    m_context.dispatch(groupCountX, groupCountY, groupCountZ);
}

TransferCommands::TransferCommands(CommandContext& context)
    : ScopedCommands(context) {
}

void TransferCommands::write_image(Image* image, const void* src, size_t size) const {
    DUK_ASSERT(valid());
    m_context.write_image(image, src, size);
}

void TransferCommands::write_frame_buffer(FrameBuffer* frameBuffer, const Image* const* attachments, uint32_t attachmentCount) const {
    DUK_ASSERT(valid());
    m_context.write_frame_buffer(frameBuffer, attachments, attachmentCount);
}

void TransferCommands::write_buffer(Buffer* buffer, const void* src, size_t size, size_t offset) const {
    DUK_ASSERT(valid());
    m_context.write_buffer(buffer, src, size, offset);
}

void TransferCommands::read_buffer(Buffer* buffer, void* dst, size_t size, size_t offset) const {
    DUK_ASSERT(valid());
    m_context.read_buffer(buffer, dst, size, offset);
}

CommandContext::~CommandContext() = default;

RenderCommands CommandContext::render(const RenderBeginParams& params) {
    return RenderCommands(*this, params);
}

ComputeCommands CommandContext::compute() {
    return ComputeCommands(*this);
}

TransferCommands CommandContext::transfer() {
    return TransferCommands(*this);
}

}// namespace duk::rhi
