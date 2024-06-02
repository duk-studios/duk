//
// Created by Ricardo on 21/04/2024.
//

#ifndef DUK_RENDERER_MESH_BUFFER_H
#define DUK_RENDERER_MESH_BUFFER_H

#include <duk_renderer/vertex_types.h>
#include <duk_rhi/index_types.h>
#include <duk_rhi/rhi.h>
#include <duk_tools/fixed_vector.h>

namespace duk::renderer {

struct MeshBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    duk::rhi::IndexType indexType;
    duk::rhi::VertexLayout vertexLayout;
    duk::rhi::Buffer::UpdateFrequency updateFrequency;
};

class MeshBuffer {
private:
    struct ManagedBufferCreateInfo {
        duk::rhi::RHI* rhi;
        duk::rhi::CommandQueue* commandQueue;
        duk::rhi::Buffer::Type type;
        duk::rhi::Buffer::UpdateFrequency updateFrequency;
        size_t elementCount;
        size_t elementSize;
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

        uint32_t allocate(size_t size);

        void free(uint32_t handle);

        void write(uint32_t handle, const void* src, size_t size, size_t offset) const;

        uint8_t* write_ptr(uint32_t handle) const;

        DUK_NO_DISCARD Block at(uint32_t handle) const;

        DUK_NO_DISCARD size_t size_of(uint32_t handle) const;

        DUK_NO_DISCARD size_t offset_of(uint32_t handle) const;

        DUK_NO_DISCARD size_t element_size() const;

        DUK_NO_DISCARD rhi::Buffer* internal_buffer();

        DUK_NO_DISCARD const rhi::Buffer* internal_buffer() const;

    private:
        void merge_free_blocks();

        bool allocate_from_free_blocks(uint32_t* allocationHandle, size_t size);

        void expand_by(size_t size);

    private:
        duk::rhi::RHI* m_rhi;
        std::shared_ptr<duk::rhi::Buffer> m_buffer;
        std::vector<Block> m_freeBlocks;
        std::unordered_map<uint32_t, Block> m_allocatedBlocks;
        uint32_t m_allocationCounter;
    };

public:
    explicit MeshBuffer(const MeshBufferCreateInfo& meshBufferCreateInfo);

    uint32_t allocate(uint32_t vertexCount, uint32_t indexCount);

    void free(uint32_t handle);

    void write_vertex(uint32_t handle, uint32_t bindingIndex, const void* src, size_t size, size_t offset);

    void write_index(uint32_t handle, const void* src, size_t size, size_t offset);

    uint32_t first_vertex(uint32_t handle) const;

    uint32_t first_index(uint32_t handle) const;

    void flush();

    void bind(duk::rhi::CommandBuffer* commandBuffer) const;

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

struct MeshBufferPoolCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class MeshBufferPool {
public:
    explicit MeshBufferPool(const MeshBufferPoolCreateInfo& meshBufferPoolCreateInfo);

    MeshBuffer* find_buffer(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType, rhi::Buffer::UpdateFrequency updateFrequency);

private:
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    std::unordered_map<duk::hash::Hash, std::unique_ptr<MeshBuffer>> m_meshBuffers;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MESH_BUFFER_H
