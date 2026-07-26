//
// Created by Ricardo on 23/04/2024.
//
#include <duk_renderer/sprite/sprite_cache.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_renderer/material/material.h>

namespace duk::renderer {

namespace detail {

duk::hash::Hash calculate_mesh_hash(const SpriteMetrics& spriteMetrics) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, &spriteMetrics.position.min);
    duk::hash::hash_combine(hash, &spriteMetrics.position.max);
    duk::hash::hash_combine(hash, &spriteMetrics.uv.min);
    duk::hash::hash_combine(hash, &spriteMetrics.uv.max);
    return hash;
}

std::shared_ptr<Mesh> make_sprite_mesh(rhi::CommandContext& commandContext, MeshBufferPool& meshBufferPool, const SpriteMetrics& spriteMetrics) {
    static const VertexAttributes s_vertexAttributes({VertexAttributes::POSITION, VertexAttributes::UV});
    auto& meshBuffer = *meshBufferPool.find_buffer(commandContext, s_vertexAttributes.vertex_layout(), rhi::IndexType::UINT32, rhi::Buffer::UpdateFrequency::DYNAMIC);

    const auto meshHandle = meshBuffer.allocate(commandContext, 4, 6);
    const auto [position, uv] = spriteMetrics;
    {
        const auto& posMin = position.min;
        const auto& posMax = position.max;

        std::array positions = {glm::vec3(posMin.x, posMin.y, 0.0f), glm::vec3(posMax.x, posMin.y, 0.0f), glm::vec3(posMin.x, posMax.y, 0.0f), glm::vec3(posMax.x, posMax.y, 0.0f)};

        const auto block = meshBuffer.vertex_at(meshHandle, VertexAttributes::POSITION).value();
        block.buffer->write(positions.data(), block.offset, positions.size() * sizeof(glm::vec3));
    }

    {
        const auto& uvMin = uv.min;
        const auto& uvMax = uv.max;

        std::array uvs = {glm::vec2(uvMin.x, uvMax.y), glm::vec2(uvMax.x, uvMax.y), glm::vec2(uvMin.x, uvMin.y), glm::vec2(uvMax.x, uvMin.y)};

        const auto block = meshBuffer.vertex_at(meshHandle, VertexAttributes::UV).value();
        block.buffer->write(uvs.data(), block.offset, uvs.size() * sizeof(glm::vec2));
    }

    {
        std::array indices = {0, 2, 1, 2, 3, 1};

        const auto block = meshBuffer.index_at(meshHandle).value();
        block.buffer->write(indices.data(), block.offset, indices.size() * sizeof(uint32_t));
    }

    return std::make_shared<Mesh>(meshBuffer, meshHandle, 4, 6);
}


}// namespace detail

SpriteCache::SpriteCache(std::shared_ptr<MeshBufferPool> meshBufferPool)
    : m_meshBufferPool(std::move(meshBufferPool)) {
}

const Mesh* SpriteCache::mesh_for(rhi::CommandContext& commandContext, const SpriteMetrics& spriteMetrics) {
    const auto hash = detail::calculate_mesh_hash(spriteMetrics);
    if (const auto it = m_meshes.find(hash); it != m_meshes.end()) {
        return it->second.get();
    }

    auto mesh = detail::make_sprite_mesh(commandContext, *m_meshBufferPool, spriteMetrics);

    auto [it, inserted] = m_meshes.emplace(hash, mesh);
    if (!inserted) {
        return nullptr;
    }
    return it->second.get();
}

void SpriteCache::clear() {
    m_meshes.clear();
}

}// namespace duk::renderer
