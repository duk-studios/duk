//
// Created by Ricardo on 21/04/2024.
//

#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_log/log.h>

namespace duk::renderer {

static constexpr size_t kBufferElementBlockCount = 1024;

namespace detail {

static duk::hash::Hash calculate_hash(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType, rhi::Buffer::UpdateFrequency updateFrequency) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, vertexLayout);
    duk::hash::hash_combine(hash, indexType);
    duk::hash::hash_combine(hash, updateFrequency);
    return hash;
}

}// namespace detail

MeshBuffer::ManagedBuffer::ManagedBuffer(const MeshBuffer::ManagedBufferCreateInfo& managedBufferCreateInfo)
    : m_rhi(managedBufferCreateInfo.rhi)
    , m_allocationCounter(0) {
    m_buffer = m_rhi->create_buffer({.type = managedBufferCreateInfo.type,
                                     .updateFrequency = managedBufferCreateInfo.updateFrequency,
                                     .elementCount = managedBufferCreateInfo.elementCount,
                                     .elementSize = managedBufferCreateInfo.elementSize,
                                     .commandQueue = managedBufferCreateInfo.commandQueue});

    m_freeBlocks.push_back({.offset = 0, .size = m_buffer->byte_size()});
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
    auto elementCount = size / m_buffer->element_size();
    auto newBlockElementCount = (elementCount / kBufferElementBlockCount + 1) * kBufferElementBlockCount;
    expand_by_element_count(newBlockElementCount);
    duk::log::info("expanding managed buffer by {} elements", newBlockElementCount);

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

void MeshBuffer::ManagedBuffer::write(uint32_t handle, const void* src, size_t size, size_t offset) const {
    const auto allocatedBlock = at(handle);

    // make sure to not write on other allocations space
    DUK_ASSERT(size + offset <= allocatedBlock.size);

    m_buffer->write(src, size, allocatedBlock.offset + offset);
}

uint8_t* MeshBuffer::ManagedBuffer::write_ptr(uint32_t handle) const {
    return m_buffer->write_ptr(offset_of(handle));
}

MeshBuffer::ManagedBuffer::Block MeshBuffer::ManagedBuffer::at(uint32_t handle) const {
    auto it = m_allocatedBlocks.find(handle);
    if (it == m_allocatedBlocks.end()) {
        throw std::logic_error("tried to access an invalid mesh handle");
    }
    return it->second;
}

size_t MeshBuffer::ManagedBuffer::size_of(uint32_t handle) const {
    auto it = m_allocatedBlocks.find(handle);
    if (it == m_allocatedBlocks.end()) {
        throw std::logic_error("tried to get size of invalid handle of ManagedBuffer");
    }
    return it->second.size;
}

size_t MeshBuffer::ManagedBuffer::offset_of(uint32_t handle) const {
    auto it = m_allocatedBlocks.find(handle);
    if (it == m_allocatedBlocks.end()) {
        throw std::logic_error("tried to get offset of invalid handle of ManagedBuffer");
    }
    return it->second.offset;
}

rhi::Buffer* MeshBuffer::ManagedBuffer::internal_buffer() {
    return m_buffer.get();
}

const rhi::Buffer* MeshBuffer::ManagedBuffer::internal_buffer() const {
    return m_buffer.get();
}

size_t MeshBuffer::ManagedBuffer::element_size() const {
    return m_buffer->element_size();
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
    for (auto& currentBlock: m_freeBlocks) {
        // if current block is not contiguous, store previous accumulated blocks and start to count a new block
        // starting from currentBlock.offset
        if ((blockOffset + blockSize) != currentBlock.offset) {
            // only insert block if it has some size
            // e.g. the first block might not start at 0
            if (blockSize > 0) {
                freeBlocks.push_back({.offset = blockOffset, .size = blockSize});
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
        freeBlocks.push_back({.offset = blockOffset, .size = blockSize});
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

            auto handle = ++m_allocationCounter;

            m_allocatedBlocks.emplace(handle, allocatedBlock);

            *allocationHandle = handle;
            return true;
        }
    }
    return false;
}

void MeshBuffer::ManagedBuffer::expand_by_element_count(size_t count) {
    // clang-format off
    auto newBuffer = m_rhi->create_buffer({
        .type = m_buffer->type(),
        .updateFrequency = m_buffer->update_frequency(),
        .elementCount = m_buffer->element_count() + count,
        .elementSize = m_buffer->element_size(),
        .commandQueue = m_buffer->command_queue()
    });
    // clang-format on

    newBuffer->copy_from(m_buffer.get(), m_buffer->byte_size(), 0, 0);

    m_freeBlocks.push_back({.offset = m_buffer->byte_size(), .size = count * m_buffer->element_size()});

    merge_free_blocks();

    std::swap(m_buffer, newBuffer);
}

MeshBuffer::MeshBuffer(const MeshBufferCreateInfo& meshBufferCreateInfo)
    : m_vertexLayout(meshBufferCreateInfo.vertexLayout)
    , m_indexType(meshBufferCreateInfo.indexType)
    , m_allocationCounter(0) {
    m_vertexBuffers.resize(m_vertexLayout.size());
    for (auto i = 0; i < m_vertexBuffers.size(); i++) {
        const auto format = meshBufferCreateInfo.vertexLayout.format_at(i);
        if (format == rhi::VertexInput::Format::UNDEFINED) {
            continue;
        }
        ManagedBufferCreateInfo vertexBufferCreateInfo = {};
        vertexBufferCreateInfo.rhi = meshBufferCreateInfo.rhi;
        vertexBufferCreateInfo.commandQueue = meshBufferCreateInfo.commandQueue;
        vertexBufferCreateInfo.elementSize = duk::rhi::VertexInput::size_of(format);
        vertexBufferCreateInfo.elementCount = kBufferElementBlockCount;
        vertexBufferCreateInfo.updateFrequency = meshBufferCreateInfo.updateFrequency;
        vertexBufferCreateInfo.type = rhi::Buffer::Type::VERTEX;
        m_vertexBuffers[i] = std::make_unique<ManagedBuffer>(vertexBufferCreateInfo);
    }

    if (meshBufferCreateInfo.indexType != rhi::IndexType::NONE) {
        ManagedBufferCreateInfo indexBufferCreateInfo = {};
        indexBufferCreateInfo.rhi = meshBufferCreateInfo.rhi;
        indexBufferCreateInfo.commandQueue = meshBufferCreateInfo.commandQueue;
        indexBufferCreateInfo.elementSize = rhi::index_size(m_indexType);
        indexBufferCreateInfo.elementCount = kBufferElementBlockCount;
        indexBufferCreateInfo.updateFrequency = meshBufferCreateInfo.updateFrequency;
        indexBufferCreateInfo.type = rhi::buffer_type_from_index_type(m_indexType);

        m_indexBuffer = std::make_unique<ManagedBuffer>(indexBufferCreateInfo);
    }
}

uint32_t MeshBuffer::allocate(uint32_t vertexCount, uint32_t indexCount) {
    auto handle = ++m_allocationCounter;
    Allocation allocation = {};
    allocation.vertexHandles.resize(m_vertexBuffers.size(), 0);
    for (auto i = 0; i < m_vertexBuffers.size(); i++) {
        auto vertexBuffer = m_vertexBuffers[i].get();
        if (!vertexBuffer) {
            continue;
        }
        allocation.vertexHandles[i] = vertexBuffer->allocate(vertexCount * vertexBuffer->element_size());

        if (i == 0) {
            allocation.firstVertex = vertexBuffer->offset_of(allocation.vertexHandles[i]) / vertexBuffer->element_size();
        }
    }
    allocation.indexHandle = m_indexBuffer ? m_indexBuffer->allocate(indexCount * m_indexBuffer->element_size()) : 0;
    if (allocation.indexHandle) {
        allocation.firstIndex = m_indexBuffer->offset_of(allocation.indexHandle) / m_indexBuffer->element_size();
    }

    m_allocations.emplace(handle, allocation);

    return handle;
}

void MeshBuffer::free(uint32_t handle) {
    auto& allocation = m_allocations.at(handle);
    for (auto i = 0; i < allocation.vertexHandles.size(); i++) {
        auto& vertexBufferHandle = allocation.vertexHandles[i];
        if (vertexBufferHandle) {
            auto& vertexBuffer = m_vertexBuffers.at(i);
            vertexBuffer->free(vertexBufferHandle);
            vertexBufferHandle = 0;
        }
    }

    if (allocation.indexHandle) {
        m_indexBuffer->free(allocation.indexHandle);
        allocation.indexHandle = 0;
    }
}

void MeshBuffer::write_vertex(uint32_t handle, uint32_t bindingIndex, const void* src, size_t size, size_t offset) {
    auto& allocation = m_allocations.at(handle);
    auto& vertexBufferHandle = allocation.vertexHandles[bindingIndex];
    if (!vertexBufferHandle) {
        duk::log::fatal("tried to write to an vertex buffer that was not allocated");
        return;
    }
    auto& vertexBuffer = m_vertexBuffers.at(bindingIndex);
    vertexBuffer->write(vertexBufferHandle, src, size, offset);
}

void MeshBuffer::write_index(uint32_t handle, const void* src, size_t size, size_t offset) {
    auto& allocation = m_allocations.at(handle);
    if (!allocation.indexHandle) {
        duk::log::fatal("tried to write to an index buffer that was not allocated");
        return;
    }
    m_indexBuffer->write(allocation.indexHandle, src, size, offset);
}

uint32_t MeshBuffer::first_vertex(uint32_t handle) const {
    auto& allocation = m_allocations.at(handle);
    return allocation.firstVertex;
}

uint32_t MeshBuffer::first_index(uint32_t handle) const {
    auto& allocation = m_allocations.at(handle);
    return allocation.firstIndex;
}

duk::rhi::VertexLayout MeshBuffer::vertex_layout() const {
    return m_vertexLayout;
}

duk::rhi::IndexType MeshBuffer::index_type() const {
    return m_indexType;
}

void MeshBuffer::flush() {
    for (auto& vertexBuffer: m_vertexBuffers) {
        if (vertexBuffer) {
            vertexBuffer->internal_buffer()->flush();
        }
    }

    if (m_indexBuffer) {
        m_indexBuffer->internal_buffer()->flush();
    }
}

void MeshBuffer::bind(duk::rhi::CommandBuffer* commandBuffer) const {
    duk::tools::FixedVector<const duk::rhi::Buffer*, 32> buffers;
    for (auto i = 0; i < m_vertexLayout.size(); i++) {
        if (const auto vertexBuffer = m_vertexBuffers[i].get()) {
            buffers.emplace_back(vertexBuffer->internal_buffer());
        } else {
            buffers.emplace_back(nullptr);
        }
    }
    commandBuffer->bind_vertex_buffer(buffers.data(), buffers.size(), 0);

    if (m_indexBuffer) {
        commandBuffer->bind_index_buffer(m_indexBuffer->internal_buffer());
    }
}

MeshBufferPool::MeshBufferPool(const MeshBufferPoolCreateInfo& meshBufferPoolCreateInfo)
    : m_rhi(meshBufferPoolCreateInfo.rhi)
    , m_commandQueue(meshBufferPoolCreateInfo.commandQueue) {
}

MeshBuffer* MeshBufferPool::find_buffer(const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType, rhi::Buffer::UpdateFrequency updateFrequency) {
    auto hash = detail::calculate_hash(vertexLayout, indexType, updateFrequency);

    auto it = m_meshBuffers.find(hash);
    if (it == m_meshBuffers.end()) {
        MeshBufferCreateInfo meshBufferCreateInfo = {};
        meshBufferCreateInfo.rhi = m_rhi;
        meshBufferCreateInfo.commandQueue = m_commandQueue;
        meshBufferCreateInfo.indexType = indexType;
        meshBufferCreateInfo.vertexLayout = vertexLayout;
        meshBufferCreateInfo.updateFrequency = updateFrequency;

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

}// namespace duk::renderer