//
// Created by Ricardo on 24/04/2024.
//
#include <duk_renderer/sprite/sprite_mesh.h>
#include <duk_renderer/mesh_buffer.h>

namespace duk::renderer {

static const VertexAttributes s_vertexAttributes({VertexAttributes::POSITION, VertexAttributes::UV});

SpriteMesh::SpriteMesh(const SpriteMeshCreateInfo& spriteMeshCreateInfo) {
    const auto meshBufferPool = spriteMeshCreateInfo.meshBufferPool;
    const auto spriteAtlas = spriteMeshCreateInfo.sprite;
    const auto spriteIndex = spriteMeshCreateInfo.spriteIndex;
    const auto [position, uv] = spriteAtlas->compute_metrics(spriteIndex);

    {
        m_meshBuffer = meshBufferPool->find_buffer(s_vertexAttributes.vertex_layout(), rhi::IndexType::UINT32, rhi::Buffer::UpdateFrequency::DYNAMIC);
    }

    m_meshHandle = m_meshBuffer->allocate(4, 6);

    m_firstVertex = m_meshBuffer->first_vertex(m_meshHandle);
    m_firstIndex = m_meshBuffer->first_index(m_meshHandle);

    {
        const auto& posMin = position.min;
        const auto& posMax = position.max;

        std::array positions = {glm::vec3(posMin.x, posMin.y, 0.0f), glm::vec3(posMax.x, posMin.y, 0.0f), glm::vec3(posMin.x, posMax.y, 0.0f), glm::vec3(posMax.x, posMax.y, 0.0f)};

        m_meshBuffer->write_vertex(m_meshHandle, VertexAttributes::POSITION, positions.data(), positions.size() * sizeof(glm::vec3), 0);
    }

    {
        const auto& uvMin = uv.min;
        const auto& uvMax = uv.max;

        std::array uvs = {glm::vec2(uvMin.x, uvMax.y), glm::vec2(uvMax.x, uvMax.y), glm::vec2(uvMin.x, uvMin.y), glm::vec2(uvMax.x, uvMin.y)};

        m_meshBuffer->write_vertex(m_meshHandle, VertexAttributes::UV, uvs.data(), uvs.size() * sizeof(glm::vec2), 0);
    }

    {
        std::array indices = {0, 2, 1, 2, 3, 1};

        m_meshBuffer->write_index(m_meshHandle, indices.data(), indices.size() * sizeof(uint32_t), 0);
    }

    m_meshBuffer->flush();
}

SpriteMesh::~SpriteMesh() {
    m_meshBuffer->free(m_meshHandle);
}

void SpriteMesh::draw(duk::rhi::CommandBuffer* commandBuffer, uint32_t instanceCount, uint32_t firstInstance) const {
    m_meshBuffer->bind(commandBuffer);
    commandBuffer->draw_indexed(6, instanceCount, m_firstIndex, static_cast<int32_t>(m_firstVertex), firstInstance);
}

}// namespace duk::renderer