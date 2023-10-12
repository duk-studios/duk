/// 04/08/2023
/// mesh_pool.h

#ifndef DUK_RENDERER_MESH_POOL_H
#define DUK_RENDERER_MESH_POOL_H

#include <duk_renderer/brushes/mesh_data_source.h>
#include <duk_renderer/brushes/brush.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/rhi.h>

#include <unordered_map>

namespace duk::renderer {

class MeshBuffer;
class MeshBufferPool;

struct MeshCreateInfo {
    MeshBufferPool* meshBufferPool;
    MeshDataSource* meshDataSource;
};

class Mesh : public Brush {
public:

    explicit Mesh(const MeshCreateInfo& meshCreateInfo);

    ~Mesh() override;

    DUK_NO_DISCARD rhi::VertexLayout vertex_layout() const;

    DUK_NO_DISCARD rhi::IndexType index_type() const;

    void draw(rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance);

private:

    void create(MeshDataSource* meshDataSource);

private:
    MeshBufferPool* m_meshBufferPool;
    uint32_t m_vertexBufferHandle;
    uint32_t m_indexBufferHandle;
    MeshBuffer* m_currentBuffer;
    size_t m_firstVertex;
    size_t m_vertexCount;
    size_t m_firstIndex;
    size_t m_indexCount;
    rhi::IndexType m_indexType;
    rhi::VertexLayout m_vertexLayout;

};

struct MeshBufferCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    duk::rhi::IndexType indexType;
    duk::rhi::VertexLayout vertexLayout;
};

class MeshBuffer {
private:

    struct ManagedBufferCreateInfo {
        duk::rhi::RHI* rhi;
        duk::rhi::CommandQueue* commandQueue;
        duk::rhi::Buffer::Type type;
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

        void write(uint32_t handle, uint8_t* src, size_t size);

        uint8_t* write_ptr(uint32_t handle);

        Block at(uint32_t handle);

        DUK_NO_DISCARD rhi::Buffer* internal_buffer();

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

    DUK_NO_DISCARD ManagedBuffer* vertex_buffer();

    DUK_NO_DISCARD ManagedBuffer* index_buffer();

private:
    std::unique_ptr<ManagedBuffer> m_vertexBuffer;
    std::unique_ptr<ManagedBuffer> m_indexBuffer;
};

struct MeshBufferPoolCreateInfo {
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
};

class MeshBufferPool {
public:

    explicit MeshBufferPool(const MeshBufferPoolCreateInfo& meshBufferPoolCreateInfo);

    std::shared_ptr<Mesh> create_mesh(MeshDataSource* meshDataSource);

    MeshBuffer* find_buffer(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType);

private:
    duk::rhi::RHI* m_rhi;
    duk::rhi::CommandQueue* m_commandQueue;
    std::unordered_map<uint32_t, std::unique_ptr<MeshBuffer>> m_meshBuffers;
};

}

#endif // DUK_RENDERER_MESH_POOL_H

