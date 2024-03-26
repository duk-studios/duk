//
// Created by rov on 1/9/2024.
//

#include <duk_objects/objects.h>
#include <duk_renderer/brushes/sprite_brush.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/renderer.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/index_types.h>
#include <duk_tools/fixed_vector.h>

#include <numbers>

namespace duk::renderer {

namespace detail {

static const uint32_t kPixelsPerUnit = 100;
static const uint32_t kVerticesPerSprite = 4;
static const uint32_t kIndicesPerSprite = 6;

static std::shared_ptr<duk::rhi::Buffer> create_vertex_buffer(Renderer* renderer, size_t spriteCount, VertexAttributes::Type attribute) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.type = duk::rhi::Buffer::Type::VERTEX;
    bufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    bufferCreateInfo.commandQueue = renderer->main_command_queue();
    bufferCreateInfo.elementCount = spriteCount * detail::kVerticesPerSprite;
    bufferCreateInfo.elementSize = VertexAttributes::size_of(attribute);
    return renderer->rhi()->create_buffer(bufferCreateInfo);
}

static std::shared_ptr<duk::rhi::Buffer> create_index_buffer(Renderer* renderer, size_t spriteCount) {
    duk::rhi::RHI::BufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.type = duk::rhi::Buffer::Type::INDEX_32;
    bufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    bufferCreateInfo.commandQueue = renderer->main_command_queue();
    bufferCreateInfo.elementCount = spriteCount * detail::kIndicesPerSprite;
    bufferCreateInfo.elementSize = duk::rhi::index_size(duk::rhi::IndexType::UINT32);
    return renderer->rhi()->create_buffer(bufferCreateInfo);
}

}// namespace detail

SpriteBrush::SpriteBrush(const SpriteBrushCreateInfo& spriteBrushCreateInfo)
    : m_spriteCapacity(spriteBrushCreateInfo.initialSpriteCapacity)
    , m_renderer(spriteBrushCreateInfo.renderer)
    , m_spriteCount(0) {
    m_vertexBuffers.at(VertexAttributes::POSITION) = detail::create_vertex_buffer(m_renderer, m_spriteCapacity, VertexAttributes::POSITION);
    m_vertexBuffers.at(VertexAttributes::UV) = detail::create_vertex_buffer(m_renderer, m_spriteCapacity, VertexAttributes::UV);
    m_indexBuffer = detail::create_index_buffer(m_renderer, m_spriteCapacity);
}

void SpriteBrush::push(const Sprite* sprite, const glm::mat4& model) {
    const auto currentSpriteIndex = m_spriteCount++;
    if (m_spriteCount >= m_spriteCapacity) {
        resize(static_cast<uint32_t>(static_cast<double>(m_spriteCapacity) * std::numbers::phi_v<double>));
    }

    glm::vec2 halfSize = {static_cast<float>(sprite->width()) / detail::kPixelsPerUnit / 2, static_cast<float>(sprite->height()) / detail::kPixelsPerUnit / 2};

    // position
    {
        std::array<glm::vec3, 4> positions = {};
        positions[0] = model * glm::vec4(-halfSize.x, -halfSize.y, 0.0f, 1.0f);
        positions[1] = model * glm::vec4(halfSize.x, -halfSize.y, 0.0f, 1.0f);
        positions[2] = model * glm::vec4(-halfSize.x, halfSize.y, 0.0f, 1.0f);
        positions[3] = model * glm::vec4(halfSize.x, halfSize.y, 0.0f, 1.0f);

        auto positionVertexBuffer = m_vertexBuffers.at(VertexAttributes::POSITION);
        auto startVertex = currentSpriteIndex * detail::kVerticesPerSprite;
        positionVertexBuffer->write(positions, startVertex);
    }

    // uv
    {
        auto uvMin = sprite->image_min();
        auto uvMax = sprite->image_max();

        std::array<glm::vec2, 4> uvs = {};
        uvs[0] = uvMin;
        uvs[1] = glm::vec2(uvMax.x, uvMin.y);
        uvs[2] = glm::vec2(uvMin.x, uvMax.y);
        uvs[3] = uvMax;

        auto uvVertexBuffer = m_vertexBuffers.at(VertexAttributes::UV);
        auto startVertex = currentSpriteIndex * detail::kVerticesPerSprite;
        uvVertexBuffer->write(uvs, startVertex);
    }

    // indices
    {
        auto startIndex = currentSpriteIndex * detail::kIndicesPerSprite;
        std::array<uint32_t, 6> indices = {
                startIndex,    // 0
                startIndex + 1,// 1
                startIndex + 2,// 2
                startIndex + 2,// 2
                startIndex + 1,// 1
                startIndex + 3 // 3
        };

        m_indexBuffer->write(indices, startIndex);
    }
}

void SpriteBrush::clear() {
    m_spriteCount = 0;
}

void SpriteBrush::resize(uint32_t spriteCapacity) {
    m_spriteCapacity = spriteCapacity;
    {
        auto& positionBuffer = m_vertexBuffers.at(VertexAttributes::POSITION);
        auto buffer = detail::create_vertex_buffer(m_renderer, m_spriteCapacity, VertexAttributes::POSITION);
        buffer->copy_from(positionBuffer.get(), positionBuffer->byte_size(), 0, 0);
        positionBuffer = buffer;
    }
    {
        auto& uvVertexBuffer = m_vertexBuffers.at(VertexAttributes::UV);
        auto buffer = detail::create_vertex_buffer(m_renderer, m_spriteCapacity, VertexAttributes::UV);
        buffer->copy_from(uvVertexBuffer.get(), uvVertexBuffer->byte_size(), 0, 0);
        uvVertexBuffer = buffer;
    }
    {
        auto buffer = detail::create_index_buffer(m_renderer, m_spriteCapacity);
        buffer->copy_from(m_indexBuffer.get(), m_indexBuffer->byte_size(), 0, 0);
        m_indexBuffer = buffer;
    }
}

void SpriteBrush::draw(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) {
    assert(firstInstance + instanceCount <= m_spriteCount);
    duk::tools::FixedVector<duk::rhi::Buffer*, VertexAttributes::COUNT> buffers;

    for (auto& vertexBuffer: m_vertexBuffers) {
        duk::rhi::Buffer* buffer = nullptr;
        if (vertexBuffer) {
            buffer = vertexBuffer.get();
            buffer->flush();
        }
        buffers.push_back(buffer);
    }
    m_indexBuffer->flush();

    uint32_t indexCount = instanceCount * detail::kIndicesPerSprite;
    uint32_t firstIndex = firstInstance * detail::kIndicesPerSprite;

    commandBuffer->bind_vertex_buffer(buffers.data(), buffers.size());
    commandBuffer->bind_index_buffer(m_indexBuffer.get());
    commandBuffer->draw_indexed(indexCount, 1, firstIndex, 0, 0);
}

SpriteBrushPool::SpriteBrushPool(const SpriteBrushPoolCreateInfo& spriteBrushPoolCreateInfo)
    : m_renderer(spriteBrushPoolCreateInfo.renderer)
    , m_spriteBrushesInUse(0) {
}

SpriteBrush* SpriteBrushPool::get(uint32_t spriteHandle) {
    // see if this handle already has an associated sprite brush
    auto it = m_spriteHandleToIndex.find(spriteHandle);
    if (it != m_spriteHandleToIndex.end()) {
        return m_spriteBrushes.at(it->second).get();
    }

    // use a new sprite brush
    auto index = m_spriteBrushesInUse++;

    // check if there's a free brush and associate it
    if (m_spriteBrushesInUse < m_spriteBrushes.size()) {
        m_spriteHandleToIndex.emplace(spriteHandle, index);
        return m_spriteBrushes.at(index).get();
    }

    // otherwise allocate a new sprite brush
    SpriteBrushCreateInfo spriteBrushCreateInfo = {};
    spriteBrushCreateInfo.initialSpriteCapacity = 10;
    spriteBrushCreateInfo.renderer = m_renderer;
    auto& spriteBrush = m_spriteBrushes.emplace_back(std::make_unique<SpriteBrush>(spriteBrushCreateInfo));
    m_spriteHandleToIndex.emplace(spriteHandle, index);

    return spriteBrush.get();
}

void SpriteBrushPool::clear() {
    m_spriteBrushesInUse = 0;
    m_spriteHandleToIndex.clear();
}

}// namespace duk::renderer