/// 04/08/2023
/// mesh_pool.cpp

#include <duk_renderer/brushes/mesh.h>
#include <duk_tools/fixed_vector.h>

namespace duk::renderer {

static constexpr uint32_t kInvalidBufferHandle = std::numeric_limits<uint32_t>::max();
static constexpr size_t kBufferBlockSize = 1024;

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
        throw std::runtime_error("failed to create ManagedBuffer!");
    }

    m_buffer = expectedBuffer;

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
        throw std::runtime_error("failed to create ManagerBuffer!");
    }

    auto newBuffer = expectedBuffer;

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
        m_vertexBufferHandles({}),
        m_indexBufferHandle(kInvalidBufferHandle),
        m_currentBuffer(nullptr),
        m_firstVertex(~0),
        m_vertexCount(~0),
        m_firstIndex(~0),
        m_indexCount(~0),
        m_indexType(rhi::IndexType::NONE) {
    m_vertexBufferHandles.fill(kInvalidBufferHandle);
    create(meshCreateInfo.meshDataSource);
}

Mesh::~Mesh() {

    auto& vertexBuffers = m_currentBuffer->vertex_buffers();
    for (auto i = 0; i < m_vertexBufferHandles.size(); i++) {
        auto& vertexBufferHandle = m_vertexBufferHandles[i];
        if (vertexBufferHandle != kInvalidBufferHandle) {
            auto& vertexBuffer = vertexBuffers.at(i);
            vertexBuffer->free(vertexBufferHandle);
            vertexBufferHandle = kInvalidBufferHandle;
        }
    }

    if (m_indexBufferHandle != kInvalidBufferHandle) {
        m_currentBuffer->index_buffer()->free(m_indexBufferHandle);
        m_indexBufferHandle = kInvalidBufferHandle;
    }
}

rhi::VertexLayout Mesh::vertex_layout() const {
    return m_vertexAttributes.vertex_layout();
}

rhi::IndexType Mesh::index_type() const {
    return m_indexType;
}

void Mesh::draw(rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) {

    auto& vertexBuffers = m_currentBuffer->vertex_buffers();

    duk::tools::FixedVector<duk::rhi::Buffer*, static_cast<uint32_t>(VertexAttributes::Type::COUNT)> buffers;

    for (auto& vertexBuffer : vertexBuffers) {
        duk::rhi::Buffer* buffer = nullptr;
        if (vertexBuffer) {
            buffer = vertexBuffer->internal_buffer();
        }
        buffers.push_back(buffer);
    }

    commandBuffer->bind_vertex_buffer(buffers.data(), buffers.size());

    if (m_indexType != rhi::IndexType::NONE) {
        commandBuffer->bind_index_buffer(m_currentBuffer->index_buffer()->internal_buffer());
        commandBuffer->draw_indexed(m_indexCount, instanceCount, m_firstIndex, static_cast<int32_t>(m_firstVertex), firstInstance);
    }
    else {
        commandBuffer->draw(m_vertexCount, m_firstVertex, instanceCount, firstInstance);
    }
}

void Mesh::create(const MeshDataSource* meshDataSource) {

    m_vertexAttributes = meshDataSource->vertex_attributes();
    m_indexType = meshDataSource->index_type();

    m_currentBuffer = m_meshBufferPool->find_buffer(m_vertexAttributes, m_indexType);

    // allocate vertex buffer
    {
        auto& vertexBuffers = m_currentBuffer->vertex_buffers();

        for (auto attributeIndex : m_vertexAttributes) {
            const auto attribute = static_cast<VertexAttributes::Type>(attributeIndex);
            const auto attributeSize = VertexAttributes::size_of(attribute);
            auto& vertexBuffer = vertexBuffers[attributeIndex];
            if (!vertexBuffer) {
                throw std::invalid_argument("invalid MeshBuffer for provided MeshDataSource");
            }
            auto& vertexBufferHandle = m_vertexBufferHandles[attributeIndex];
            vertexBufferHandle = vertexBuffer->allocate(meshDataSource->vertex_count() * attributeSize);
            meshDataSource->read_vertices_attribute(attribute, vertexBuffer->write_ptr(vertexBufferHandle), meshDataSource->vertex_count(), 0);

            if (attribute == VertexAttributes::Type::POSITION) {
                const auto vertexBlock = vertexBuffer->at(vertexBufferHandle);
                m_firstVertex = vertexBlock.offset / attributeSize;
                m_vertexCount = vertexBlock.size / attributeSize;
            }

            vertexBuffer->internal_buffer()->flush();
        }
    }

    // allocate index buffer
    if (m_indexType != rhi::IndexType::NONE) {
        auto indexBuffer = m_currentBuffer->index_buffer();

        m_indexBufferHandle = indexBuffer->allocate(meshDataSource->index_byte_count());
        meshDataSource->read_indices(indexBuffer->write_ptr(m_indexBufferHandle), meshDataSource->index_count(), 0);

        const auto indexBlock = indexBuffer->at(m_indexBufferHandle);
        const auto indexByteSize = rhi::index_size(m_indexType);
        m_firstIndex = indexBlock.offset / indexByteSize;
        m_indexCount = indexBlock.size / indexByteSize;

        indexBuffer->internal_buffer()->flush();
    }
}

MeshBuffer::MeshBuffer(const MeshBufferCreateInfo& meshBufferCreateInfo)  {

    for (auto attributeIndex : meshBufferCreateInfo.vertexAttributes) {
        auto attribute = static_cast<VertexAttributes::Type>(attributeIndex);
        ManagedBufferCreateInfo vertexBufferCreateInfo = {};
        vertexBufferCreateInfo.rhi = meshBufferCreateInfo.rhi;
        vertexBufferCreateInfo.commandQueue = meshBufferCreateInfo.commandQueue;
        vertexBufferCreateInfo.elementSize = VertexAttributes::size_of(attribute);
        vertexBufferCreateInfo.elementCount = kBufferBlockSize / vertexBufferCreateInfo.elementSize;
        vertexBufferCreateInfo.type = rhi::Buffer::Type::VERTEX;

        m_vertexBuffers[attributeIndex] = std::make_unique<ManagedBuffer>(vertexBufferCreateInfo);
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

MeshBuffer::VertexBuffers& MeshBuffer::vertex_buffers() {
    return m_vertexBuffers;
}

MeshBuffer::ManagedBuffer* MeshBuffer::index_buffer() {
    return m_indexBuffer.get();
}

MeshBufferPool::MeshBufferPool(const MeshBufferPoolCreateInfo& meshBufferPoolCreateInfo) :
    m_rhi(meshBufferPoolCreateInfo.rhi),
    m_commandQueue(meshBufferPoolCreateInfo.commandQueue) {

}

std::shared_ptr<Mesh> MeshBufferPool::create_mesh(const MeshDataSource* meshDataSource) {
    MeshCreateInfo meshCreateInfo = {};
    meshCreateInfo.meshDataSource = meshDataSource;
    meshCreateInfo.meshBufferPool = this;
    return std::make_shared<Mesh>(meshCreateInfo);
}

MeshBuffer* MeshBufferPool::find_buffer(const VertexAttributes& vertexAttributes, rhi::IndexType indexType) {
    auto hash = detail::calculate_hash(vertexAttributes.vertex_layout(), indexType);

    auto it = m_meshBuffers.find(hash);
    if (it == m_meshBuffers.end()) {

        MeshBufferCreateInfo meshBufferCreateInfo = {};
        meshBufferCreateInfo.rhi = m_rhi;
        meshBufferCreateInfo.commandQueue = m_commandQueue;
        meshBufferCreateInfo.indexType = indexType;
        meshBufferCreateInfo.vertexAttributes = vertexAttributes;

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