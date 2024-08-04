//
// Created by Ricardo on 07/06/2024.
//
#include <duk_ui/text/text_mesh.h>
#include <duk_ui/text/text_mesh_builder.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/mesh/dynamic_mesh.h>

namespace duk::ui {

namespace detail {

static duk::hash::Hash calculate_hash(const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, const glm::vec2& texBoxSize) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, atlas);
    duk::hash::hash_combine(hash, text);
    duk::hash::hash_combine(hash, fontSize);
    duk::hash::hash_combine(hash, vertAlignment);
    duk::hash::hash_combine(hash, horiAlignment);
    duk::hash::hash_combine(hash, texBoxSize);
    return hash;
}

static uint32_t calculate_text_count(const std::string_view& text, bool dynamic) {
    static constexpr uint32_t kTextChunk = 64;
    if (dynamic) {
        return ((text.size() / kTextChunk) + 1) * kTextChunk;
    }
    return text.size();
}

}// namespace detail

TextMesh::TextMesh(const TextMeshCreateInfo& textMeshCreateInfo)
    : m_meshBufferPool(textMeshCreateInfo.meshBufferPool)
    , m_textCache(textMeshCreateInfo.textCache)
    , m_dynamic(textMeshCreateInfo.dynamic)
    , m_textCount(0)
    , m_hash(0) {
}

bool TextMesh::update(const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, float pixelsPerUnit) {
    auto hash = detail::calculate_hash(text, atlas, fontSize, vertAlignment, horiAlignment, textBoxSize);
    if (hash == m_hash) {
        return false;
    }
    m_hash = hash;

    const float kLineHeight = fontSize + fontSize * (1.f / 4.f);

    bool recreatedMesh = false;
    if (!m_dynamic) {
        if (const auto mesh = m_textCache->find_mesh(hash)) {
            m_mesh = mesh;
            return true;
        }
    }

    const auto textCount = detail::calculate_text_count(text, m_dynamic);

    if (!m_mesh || m_textCount < textCount || !m_dynamic) {
        m_mesh = allocate_text_mesh(m_meshBufferPool, textCount, m_dynamic);
        m_textCount = textCount;
        recreatedMesh = true;
    }

    build_text_mesh(m_mesh.get(), text, atlas, fontSize, textBoxSize, kLineHeight, vertAlignment, horiAlignment, pixelsPerUnit);

    if (!m_dynamic) {
        m_textCache->store(hash, m_mesh);
    }

    return recreatedMesh;
}

std::shared_ptr<duk::renderer::DynamicMesh> TextMesh::mesh() const {
    return m_mesh;
}

}// namespace duk::ui
