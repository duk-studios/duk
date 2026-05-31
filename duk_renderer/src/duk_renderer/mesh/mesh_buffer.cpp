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
    : m_type(managedBufferCreateInfo.type)
    , m_updateFrequency(managedBufferCreateInfo.updateFrequency)
    , m_elementSize(managedBufferCreateInfo.elementSize)
    , m_allocationCounter(0) {
    m_buffer = managedBufferCreateInfo.commandContext->create_buffer({
        .type = m_type,
        .updateFrequency = m_updateFrequency,
        .size = managedBufferCreateInfo.size,
    });
    m_freeBlocks.push_back({.offset = 0, .size = m_buffer->size()});
}

MeshBuffer::ManagedBuffer::~ManagedBuffer() {
    assert(m_allocatedBlocks.empty());
}

uint32_t MeshBuffer::ManagedBuffer::allocate(duk::rhi::CommandContext& commandContext, size_t size) {
    uint32_t allocationHandle = 0;
    if (allocate_from_free_blocks(&allocationHandle, size)) {
        return allocationHandle;
    }

    auto elementCount = size / m_elementSize;
    auto newBlockElementCount = (elementCount / kBufferElementBlockCount + 1) * kBufferElementBlockCount;
    expand_by_element_count(commandContext, newBlockElementCount);
    duk::log::verb("expanding managed buffer by {} elements", newBlockElementCount);

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

    merge_free_blocks();
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
    return m_elementSize;
}

void MeshBuffer::ManagedBuffer::merge_free_blocks() {
    if (m_freeBlocks.size() == 1) {
        return;
    }

    std::ranges::sort(m_freeBlocks, [](const Block& lhs, const Block& rhs) -> bool {
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

void MeshBuffer::ManagedBuffer::expand_by_element_count(duk::rhi::CommandContext& commandContext, size_t count) {
    auto currentSize = m_buffer->size();
    auto newBuffer = commandContext.create_buffer({
        .type = m_type,
        .updateFrequency = m_updateFrequency,
        .size = currentSize + count * m_elementSize,
    });

    commandContext.copy_to_buffer(newBuffer.get(), 0, currentSize, m_buffer.get(), 0);

    m_freeBlocks.push_back({.offset = currentSize, .size = count * m_elementSize});
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
        auto elementSize = duk::rhi::VertexInput::size_of(format);
        ManagedBufferCreateInfo vertexBufferCreateInfo = {};
        vertexBufferCreateInfo.commandContext = meshBufferCreateInfo.commandContext;
        vertexBufferCreateInfo.elementSize = elementSize;
        vertexBufferCreateInfo.size = kBufferElementBlockCount * elementSize;
        vertexBufferCreateInfo.updateFrequency = meshBufferCreateInfo.updateFrequency;
        vertexBufferCreateInfo.type = rhi::Buffer::Type::VERTEX;
        m_vertexBuffers[i] = std::make_unique<ManagedBuffer>(vertexBufferCreateInfo);
    }

    if (meshBufferCreateInfo.indexType != rhi::IndexType::NONE) {
        auto indexElementSize = rhi::index_size(m_indexType);
        ManagedBufferCreateInfo indexBufferCreateInfo = {};
        indexBufferCreateInfo.commandContext = meshBufferCreateInfo.commandContext;
        indexBufferCreateInfo.elementSize = indexElementSize;
        indexBufferCreateInfo.size = kBufferElementBlockCount * indexElementSize;
        indexBufferCreateInfo.updateFrequency = meshBufferCreateInfo.updateFrequency;
        indexBufferCreateInfo.type = rhi::Buffer::Type::INDEX;
        m_indexBuffer = std::make_unique<ManagedBuffer>(indexBufferCreateInfo);
    }
}

uint32_t MeshBuffer::allocate(duk::rhi::CommandContext& commandContext, uint32_t vertexCount, uint32_t indexCount) {
    auto handle = ++m_allocationCounter;
    Allocation allocation = {};
    allocation.vertexHandles.resize(m_vertexBuffers.size(), 0);
    for (auto i = 0; i < m_vertexBuffers.size(); i++) {
        auto vertexBuffer = m_vertexBuffers[i].get();
        if (!vertexBuffer) {
            continue;
        }
        allocation.vertexHandles[i] = vertexBuffer->allocate(commandContext, vertexCount * vertexBuffer->element_size());

        if (i == 0) {
            allocation.firstVertex = vertexBuffer->offset_of(allocation.vertexHandles[i]) / vertexBuffer->element_size();
        }
    }
    allocation.indexHandle = m_indexBuffer ? m_indexBuffer->allocate(commandContext, indexCount * m_indexBuffer->element_size()) : 0;
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
            m_vertexBuffers.at(i)->free(vertexBufferHandle);
            vertexBufferHandle = 0;
        }
    }

    if (allocation.indexHandle) {
        m_indexBuffer->free(allocation.indexHandle);
        allocation.indexHandle = 0;
    }
}

std::optional<MeshBufferBlock> MeshBuffer::vertex_at(uint32_t handle, uint32_t bindingIndex) const {
    const auto& allocation = m_allocations.at(handle);
    const auto& managed = m_vertexBuffers.at(bindingIndex);
    if (!managed || !allocation.vertexHandles[bindingIndex]) {
        return std::nullopt;
    }
    const auto block = managed->at(allocation.vertexHandles[bindingIndex]);
    return MeshBufferBlock{managed->internal_buffer(), block.offset, block.size};
}

std::optional<MeshBufferBlock> MeshBuffer::index_at(uint32_t handle) const {
    if (!m_indexBuffer || !m_allocations.at(handle).indexHandle) {
        return std::nullopt;
    }
    const auto& allocation = m_allocations.at(handle);
    const auto block = m_indexBuffer->at(allocation.indexHandle);
    return MeshBufferBlock{m_indexBuffer->internal_buffer(), block.offset, block.size};
}

uint32_t MeshBuffer::first_vertex(uint32_t handle) const {
    return m_allocations.at(handle).firstVertex;
}

uint32_t MeshBuffer::first_index(uint32_t handle) const {
    return m_allocations.at(handle).firstIndex;
}

duk::rhi::VertexLayout MeshBuffer::vertex_layout() const {
    return m_vertexLayout;
}

duk::rhi::IndexType MeshBuffer::index_type() const {
    return m_indexType;
}

MeshBuffer* MeshBufferPool::find_buffer(duk::rhi::CommandContext& commandContext, const rhi::VertexLayout& vertexLayout, rhi::IndexType indexType, rhi::Buffer::UpdateFrequency updateFrequency) {
    auto hash = detail::calculate_hash(vertexLayout, indexType, updateFrequency);

    auto it = m_meshBuffers.find(hash);
    if (it == m_meshBuffers.end()) {
        MeshBufferCreateInfo meshBufferCreateInfo = {};
        meshBufferCreateInfo.commandContext = &commandContext;
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

duk::rhi::ShaderInput make_shader_input(const MeshBuffer& meshBuffer, uint32_t handle) {
    duk::rhi::ShaderInput input = {};
    const auto vertexLayout = meshBuffer.vertex_layout();
    for (auto i = 0u; i < static_cast<uint32_t>(vertexLayout.size()); i++) {
        if (const auto block = meshBuffer.vertex_at(handle, i)) {
            input.vertex[i] = {block->buffer, static_cast<uint32_t>(block->offset)};
        }
    }
    if (const auto block = meshBuffer.index_at(handle)) {
        input.index.resource = {block->buffer, static_cast<uint32_t>(block->offset)};
        input.index.type = meshBuffer.index_type();
    }
    return input;
}

}// namespace duk::renderer