/// 04/08/2023
/// mesh_pool.cpp

#include <duk_renderer/mesh.h>

namespace duk::renderer {

namespace detail {

static duk::hash::Hash calculate_hash(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, vertexLayout);
    duk::hash::hash_combine(hash, indexType);
    return hash;
}

}

MeshBuffer::ManagedBuffer::ManagedBuffer(const MeshBuffer::ManagedBufferCreateInfo& managedBufferCreateInfo) :
    m_rhi(managedBufferCreateInfo.rhi),
    m_allocationCounter(0) {

    auto expectedBuffer = m_rhi->create_buffer({
        .type = managedBufferCreateInfo.type,
        .updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC,
        .elementCount = managedBufferCreateInfo.elementCount,
        .elementSize = managedBufferCreateInfo.elementSize,
        .commandQueue = managedBufferCreateInfo.commandQueue
    });

    if (!expectedBuffer) {
        throw std::runtime_error("failed to create ManagedBuffer: " + expectedBuffer.error().description());
    }

    m_buffer = std::move(expectedBuffer.value());

    m_freeBlocks.push_back({
        .offset = 0,
        .size = m_buffer->byte_size()
    });
}

MeshBuffer::ManagedBuffer::~ManagedBuffer() {
    assert(m_allocatedBlocks.empty());
}

uint32_t MeshBuffer::ManagedBuffer::allocate(size_t size) {

    // try to allocate from current free blocks
    uint32_t allocationHandle = 0;
    if (allocate_from_free_blocks(&allocationHandle, size)) {
        return allocationHandle;
    }

    // if we fail, expand the buffer by the given size
    expand_by(size);

    // try to allocate again
    if (!allocate_from_free_blocks(&allocationHandle, size)) {

        // this should never happen, but if it does throw an error
        throw std::runtime_error("failed to allocate mesh in ManagedBuffer");
    }

    return allocationHandle;
}

void MeshBuffer::ManagedBuffer::free(uint32_t handle) {
    auto it = m_allocatedBlocks.find(handle);

    if (it == m_allocatedBlocks.end()) {
        throw std::logic_error("tried to free an invalid mesh handle");
    }

    auto block = it->second;

    m_allocatedBlocks.erase(it);

    m_freeBlocks.emplace_back(block);

    /// merge contiguous free blocks
    merge_free_blocks();
}

void MeshBuffer::ManagedBuffer::write(uint32_t handle, uint8_t* src, size_t size) {

    const auto allocatedBlock = at(handle);

    // make sure to not write on other allocations space
    assert(size <= allocatedBlock.size);

    m_buffer->write(src, size, allocatedBlock.offset);
}

uint8_t* MeshBuffer::ManagedBuffer::write_ptr(uint32_t handle) {
    const auto allocatedBlock = at(handle);
    return m_buffer->write_ptr(allocatedBlock.offset);
}

rhi::Buffer* MeshBuffer::ManagedBuffer::internal_buffer() {
    return m_buffer.get();
}

void MeshBuffer::ManagedBuffer::merge_free_blocks() {
    if (m_freeBlocks.size() == 1) {
        return;
    }

    // sort to facilitate merging
    std::sort(m_freeBlocks.begin(), m_freeBlocks.end(), [](const Block& lhs, const Block& rhs) -> bool {
        return lhs.offset < rhs.offset;
    });

    decltype(m_freeBlocks) freeBlocks;
    size_t blockSize = 0;
    size_t blockOffset = 0;
    for (auto& currentBlock : m_freeBlocks) {
        // if current block is not contiguous, store previous accumulated blocks and start to count a new block
        // starting from currentBlock.offset
        if ((blockOffset + blockSize) != currentBlock.offset) {

            // only insert block if it has some size
            // e.g. the first block might not start at 0
            if (blockSize > 0) {
                freeBlocks.push_back({
                    .offset = blockOffset,
                    .size = blockSize
                });
            }

            // reset block size
            blockSize = 0;

            // start accumulating new block from the start of currentBlock
            blockOffset = currentBlock.offset;
        }

        // accumulate block size
        blockSize += currentBlock.size;
    }

    if (blockSize > 0) {
        freeBlocks.push_back({
            .offset = blockOffset,
            .size = blockSize
        });
    }

    std::swap(m_freeBlocks, freeBlocks);
}

bool MeshBuffer::ManagedBuffer::allocate_from_free_blocks(uint32_t* allocationHandle, size_t size) {
    for (int i = static_cast<int>(m_freeBlocks.size()) - 1; i >= 0; i--) {
        auto& block = m_freeBlocks[i];

        if (block.size >= size) {
            // use this block for the allocation

            Block allocatedBlock = {block.offset, size};

            block.size -= size;
            block.offset += size;
            if (block.size == 0) {
                m_freeBlocks.erase(m_freeBlocks.begin() + i);
            }

            auto handle = m_allocationCounter++;

            m_allocatedBlocks.emplace(handle, allocatedBlock);

            *allocationHandle = handle;
            return true;
        }
    }
    return false;
}

void MeshBuffer::ManagedBuffer::expand_by(size_t size) {

    auto expectedBuffer = m_rhi->create_buffer({
        .type = m_buffer->type(),
        .updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC,
        .elementCount = (m_buffer->byte_size() + size) / m_buffer->element_size(),
        .elementSize = m_buffer->element_size(),
        .commandQueue = m_buffer->command_queue()
    });

    if (!expectedBuffer) {
        throw std::runtime_error("failed to create ManagerBuffer: " + expectedBuffer.error().description());
    }

    auto newBuffer = std::move(expectedBuffer.value());

    newBuffer->copy_from(m_buffer.get(), m_buffer->byte_size(), 0, 0);

    m_freeBlocks.push_back({
        .offset = m_buffer->byte_size(),
        .size = size
    });

    merge_free_blocks();

    std::swap(m_buffer, newBuffer);
}

MeshBuffer::ManagedBuffer::Block MeshBuffer::ManagedBuffer::at(uint32_t handle) {
    auto it = m_allocatedBlocks.find(handle);

    if (it == m_allocatedBlocks.end()) {
        throw std::logic_error("invalid handle used to write on ManagedBuffer");
    }

    return it->second;
}

Mesh::Mesh(const MeshCreateInfo& meshCreateInfo) :
        m_meshBufferPool(meshCreateInfo.meshBufferPool),
        m_vertexBufferHandle(kInvalidBufferHandle),
        m_indexBufferHandle(kInvalidBufferHandle),
        m_currentBuffer(nullptr),
        m_firstVertex(~0),
        m_vertexCount(~0),
        m_firstIndex(~0),
        m_indexCount(~0),
        m_indexType(rhi::IndexType::NONE) {

    create(meshCreateInfo.meshDataSource);
}

Mesh::~Mesh() {

    if (m_vertexBufferHandle != kInvalidBufferHandle) {
        m_currentBuffer->vertex_buffer()->free(m_vertexBufferHandle);
        m_vertexBufferHandle = kInvalidBufferHandle;
    }

    if (m_indexBufferHandle != kInvalidBufferHandle) {
        m_currentBuffer->index_buffer()->free(m_indexBufferHandle);
        m_indexBufferHandle = kInvalidBufferHandle;
    }
}

rhi::VertexLayout Mesh::vertex_layout() const {
    return m_vertexLayout;
}

rhi::IndexType Mesh::index_type() const {
    return m_indexType;
}

void Mesh::draw(rhi::CommandBuffer* commandBuffer) {

    commandBuffer->bind_vertex_buffer(m_currentBuffer->vertex_buffer()->internal_buffer());

    if (m_indexType != rhi::IndexType::NONE) {
        commandBuffer->bind_index_buffer(m_currentBuffer->index_buffer()->internal_buffer());
        commandBuffer->draw_indexed(m_indexCount, 1, m_firstIndex, static_cast<int32_t>(m_firstVertex), 0);
    }
    else {
        commandBuffer->draw(m_vertexCount, m_firstVertex, 1, 0);
    }
}

void Mesh::create(MeshDataSource* meshDataSource) {

    m_vertexLayout = meshDataSource->vertex_layout();
    m_indexType = meshDataSource->index_type();

    m_currentBuffer = m_meshBufferPool->find_buffer(m_vertexLayout, m_indexType);

    // allocate vertex buffer
    {
        auto vertexBuffer = m_currentBuffer->vertex_buffer();

        m_vertexBufferHandle = vertexBuffer->allocate(meshDataSource->vertex_byte_count());
        meshDataSource->read_vertices(vertexBuffer->write_ptr(m_vertexBufferHandle), meshDataSource->vertex_byte_count(), 0);

        const auto vertexBlock = vertexBuffer->at(m_vertexBufferHandle);
        const auto vertexByteSize = m_vertexLayout.byte_size();
        m_firstVertex = vertexBlock.offset / vertexByteSize;
        m_vertexCount = vertexBlock.size / vertexByteSize;

        vertexBuffer->internal_buffer()->flush();
    }

    // allocate index buffer
    if (m_indexType != rhi::IndexType::NONE) {
        auto indexBuffer = m_currentBuffer->index_buffer();

        m_indexBufferHandle = indexBuffer->allocate(meshDataSource->index_byte_count());
        meshDataSource->read_indices(indexBuffer->write_ptr(m_indexBufferHandle), meshDataSource->index_byte_count(), 0);

        const auto indexBlock = indexBuffer->at(m_indexBufferHandle);
        const auto indexByteSize = rhi::index_size(m_indexType);
        m_firstIndex = indexBlock.offset / indexByteSize;
        m_indexCount = indexBlock.size / indexByteSize;

        indexBuffer->internal_buffer()->flush();
    }
}

MeshBuffer::MeshBuffer(const MeshBufferCreateInfo& meshBufferCreateInfo)  {

    {
        ManagedBufferCreateInfo vertexBufferCreateInfo = {};
        vertexBufferCreateInfo.rhi = meshBufferCreateInfo.rhi;
        vertexBufferCreateInfo.commandQueue = meshBufferCreateInfo.commandQueue;
        vertexBufferCreateInfo.elementSize = meshBufferCreateInfo.vertexLayout.byte_size();
        vertexBufferCreateInfo.elementCount = kBufferBlockSize / vertexBufferCreateInfo.elementSize;
        vertexBufferCreateInfo.type = rhi::Buffer::Type::VERTEX;

        m_vertexBuffer = std::make_unique<ManagedBuffer>(vertexBufferCreateInfo);
    }

    if (meshBufferCreateInfo.indexType != rhi::IndexType::NONE) {
        ManagedBufferCreateInfo indexBufferCreateInfo = {};
        indexBufferCreateInfo.rhi = meshBufferCreateInfo.rhi;
        indexBufferCreateInfo.commandQueue = meshBufferCreateInfo.commandQueue;
        indexBufferCreateInfo.elementSize = rhi::index_size(meshBufferCreateInfo.indexType);
        indexBufferCreateInfo.elementCount = kBufferBlockSize / indexBufferCreateInfo.elementSize;
        indexBufferCreateInfo.type = rhi::buffer_type_from_index_type(meshBufferCreateInfo.indexType);

        m_indexBuffer = std::make_unique<ManagedBuffer>(indexBufferCreateInfo);
    }
}

MeshBuffer::ManagedBuffer* MeshBuffer::vertex_buffer() {
    return m_vertexBuffer.get();
}

MeshBuffer::ManagedBuffer* MeshBuffer::index_buffer() {
    return m_indexBuffer.get();
}

MeshBufferPool::MeshBufferPool(const MeshBufferPoolCreateInfo& meshBufferPoolCreateInfo) :
    m_rhi(meshBufferPoolCreateInfo.rhi),
    m_commandQueue(meshBufferPoolCreateInfo.commandQueue) {

}

std::shared_ptr<Mesh> MeshBufferPool::create_mesh(MeshDataSource* meshDataSource) {
    MeshCreateInfo meshCreateInfo = {};
    meshCreateInfo.meshDataSource = meshDataSource;
    meshCreateInfo.meshBufferPool = this;
    return std::make_shared<Mesh>(meshCreateInfo);
}

MeshBuffer* MeshBufferPool::find_buffer(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType) {
    auto hash = detail::calculate_hash(vertexLayout, indexType);

    auto it = m_meshBuffers.find(hash);
    if (it == m_meshBuffers.end()) {

        MeshBufferCreateInfo meshBufferCreateInfo = {};
        meshBufferCreateInfo.rhi = m_rhi;
        meshBufferCreateInfo.commandQueue = m_commandQueue;
        meshBufferCreateInfo.indexType = indexType;
        meshBufferCreateInfo.vertexLayout = vertexLayout;

        auto [result, inserted] = m_meshBuffers.emplace(hash, std::make_unique<MeshBuffer>(meshBufferCreateInfo));
        if (!inserted) {
            throw std::runtime_error("failed to insert MeshBuffer");
        }
        if (!result->second) {
            throw std::runtime_error("failed to create MeshBuffer");
        }
        it = result;
    }

    return it->second.get();
}

}