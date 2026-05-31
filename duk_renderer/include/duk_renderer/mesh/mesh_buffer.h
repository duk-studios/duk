//
// Created by Ricardo on 21/04/2024.
//

#ifndef DUK_RENDERER_MESH_BUFFER_H
#define DUK_RENDERER_MESH_BUFFER_H

#include <duk_renderer/vertex_types.h>
#include <duk_rhi/index_types.h>
#include <duk_rhi/command_context.h>
#include <duk_tools/fixed_vector.h>
#include <optional>

namespace duk::renderer {

struct MeshBufferBlock {
    const rhi::Buffer* buffer{nullptr};
    size_t offset{0};
    size_t size{0};
};

struct MeshBufferCreateInfo {
    duk::rhi::CommandContext* commandContext;
    duk::rhi::IndexType indexType;
    duk::rhi::VertexLayout vertexLayout;
    duk::rhi::Buffer::UpdateFrequency updateFrequency;
};

class MeshBuffer {
private:
    struct ManagedBufferCreateInfo {
        duk::rhi::CommandContext* commandContext;
        duk::rhi::Buffer::Type type;
        duk::rhi::Buffer::UpdateFrequency updateFrequency;
        size_t elementSize;
        size_t size;
    };

    class ManagedBuffer {
    public:
        struct Block {
            size_t offset;
            size_t size;
        };

    public:
        explicit ManagedBuffer(const ManagedBufferCreateInfo& managedBufferCreateInfo);

        ~ManagedBuffer();

        uint32_t allocate(duk::rhi::CommandContext& commandContext, size_t size);

        void free(uint32_t handle);

        DUK_NO_DISCARD Block at(uint32_t handle) const;

        DUK_NO_DISCARD size_t size_of(uint32_t handle) const;

        DUK_NO_DISCARD size_t offset_of(uint32_t handle) const;

        DUK_NO_DISCARD size_t element_size() const;

        DUK_NO_DISCARD rhi::Buffer* internal_buffer();

        DUK_NO_DISCARD const rhi::Buffer* internal_buffer() const;

    private:
        void merge_free_blocks();

        bool allocate_from_free_blocks(uint32_t* allocationHandle, size_t size);

        void expand_by_element_count(duk::rhi::CommandContext& commandContext, size_t count);

    private:
        duk::rhi::Buffer::Type m_type;
        duk::rhi::Buffer::UpdateFrequency m_updateFrequency;
        size_t m_elementSize;
        std::shared_ptr<duk::rhi::Buffer> m_buffer;
        std::vector<Block> m_freeBlocks;
        std::unordered_map<uint32_t, Block> m_allocatedBlocks;
        uint32_t m_allocationCounter;
    };

public:
    explicit MeshBuffer(const MeshBufferCreateInfo& meshBufferCreateInfo);

    uint32_t allocate(duk::rhi::CommandContext& commandContext, uint32_t vertexCount, uint32_t indexCount);

    void free(uint32_t handle);

    DUK_NO_DISCARD uint32_t first_vertex(uint32_t handle) const;

    DUK_NO_DISCARD uint32_t first_index(uint32_t handle) const;

    DUK_NO_DISCARD duk::rhi::VertexLayout vertex_layout() const;

    DUK_NO_DISCARD duk::rhi::IndexType index_type() const;

    DUK_NO_DISCARD std::optional<MeshBufferBlock> vertex_at(uint32_t handle, uint32_t bindingIndex) const;

    DUK_NO_DISCARD std::optional<MeshBufferBlock> index_at(uint32_t handle) const;

private:
    duk::rhi::VertexLayout m_vertexLayout;
    duk::rhi::IndexType m_indexType;
    std::vector<std::unique_ptr<ManagedBuffer>> m_vertexBuffers;
    std::unique_ptr<ManagedBuffer> m_indexBuffer;

    struct Allocation {
        duk::tools::FixedVector<uint32_t, 16> vertexHandles;
        uint32_t firstVertex;
        uint32_t indexHandle;
        uint32_t firstIndex;
    };

    std::unordered_map<uint32_t, Allocation> m_allocations;
    uint32_t m_allocationCounter;
};

class MeshBufferPool {
public:
    MeshBufferPool() = default;

    MeshBuffer* find_buffer(duk::rhi::CommandContext& commandContext, const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType, rhi::Buffer::UpdateFrequency updateFrequency);

private:
    std::unordered_map<duk::hash::Hash, std::unique_ptr<MeshBuffer>> m_meshBuffers;
};

DUK_NO_DISCARD duk::rhi::ShaderInput make_shader_input(const MeshBuffer& meshBuffer, uint32_t handle);

}// namespace duk::renderer

#endif//DUK_RENDERER_MESH_BUFFER_H
