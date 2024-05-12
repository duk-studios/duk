//
// Created by Ricardo on 05/04/2024.
//

#include <duk_renderer/components/text_renderer.h>
#include <duk_renderer/renderer.h>

#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/canvas.h>

namespace duk::renderer {

namespace detail {

static std::shared_ptr<duk::rhi::Buffer> create_buffer(Renderer* renderer, size_t elementCount, size_t elementSize, duk::rhi::Buffer::Type type) {
    duk::rhi::RHI::BufferCreateInfo positionBufferCreateInfo = {};
    positionBufferCreateInfo.commandQueue = renderer->main_command_queue();
    positionBufferCreateInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    positionBufferCreateInfo.elementCount = elementCount;
    positionBufferCreateInfo.elementSize = elementSize;
    positionBufferCreateInfo.type = type;

    return renderer->rhi()->create_buffer(positionBufferCreateInfo);
}

static duk::hash::Hash calculate_hash(const FontAtlas* atlas, const std::string_view& text) {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, atlas);
    duk::hash::hash_combine(hash, text);
    return hash;
}

static constexpr uint32_t kTextChunk = 64;

}// namespace detail

TextMesh::TextMesh(const TextMeshCreateInfo& textBrushCreateInfo)
    : m_renderer(textBrushCreateInfo.renderer)
    , m_textCapacity(0)
    , m_textCount(0)
    , m_hash(0) {
    reserve(detail::kTextChunk);
}

void TextMesh::update_text(const FontAtlas* atlas, const TextRenderer& textRenderer, float pixelsPerUnit) {
    const auto& text = textRenderer.text;
    auto hash = detail::calculate_hash(atlas, text);
    if (hash == m_hash) {
        return;
    }
    m_hash = hash;

    // ensure capacity, white space will allocate memory but not use it
    if (text.size() > m_textCapacity) {
        auto newMaxTextCount = ((text.size() / m_textCapacity) + 1) * detail::kTextChunk;
        reserve(newMaxTextCount);
    }

    const float kLineHeight = textRenderer.fontSize + textRenderer.fontSize * (1.f / 4.f);

    // get words
    std::vector<std::string_view> words;
    std::string::size_type wordStart = 0;
    while (wordStart != std::string::npos && wordStart < text.size()) {
        auto chr = text[wordStart];
        std::string::size_type wordEnd;
        if (chr == '\n' || chr == ' ') {
            wordEnd = text.find_first_not_of(" \n", wordStart);
        } else {
            wordEnd = text.find_first_of(" \n", wordStart);
        }

        if (wordEnd == std::string::npos) {
            wordEnd = text.size();
        }
        words.emplace_back(text.begin() + wordStart, text.begin() + wordEnd);
        wordStart = wordEnd;
    }

    // separate them in lines
    struct Line {
        int32_t width = 0;
        std::vector<std::string_view> words;
    };

    const auto kMaxLines = textRenderer.size.y / kLineHeight;

    std::vector<Line> lines;
    lines.reserve(kMaxLines);
    auto* currentLine = &lines.emplace_back();
    for (const auto& word: words) {
        int32_t wordWidth = 0;
        int32_t lastCharSpilling = 0;
        for (auto i = 0; i < word.size(); i++) {
            auto chr = word[i];

            Glyph glyph;
            atlas->find_glyph(chr, glyph, textRenderer.fontSize);

            wordWidth += glyph.metrics.advance;
            if (i == word.size() - 1) {
                // keep track of the last character spilling,
                // basically how much the advance goes out of the character bearing + size
                // the last character of the last word in a line must not use the complete advance size
                lastCharSpilling = glyph.metrics.advance - glyph.metrics.size.x - glyph.metrics.bearing.x;
            }
        }
        if ((currentLine->width + wordWidth) >= textRenderer.size.x && !currentLine->words.empty()) {
            currentLine->width += lastCharSpilling;// remove spilling from last word
            if (lines.size() >= kMaxLines) {
                // line would be outsize of the text box, cut off text
                break;
            }
            currentLine = &lines.emplace_back();
        }
        currentLine->words.emplace_back(word);
        currentLine->width += wordWidth;
    }

    // build mesh by lines, taking alignment options into account
    m_textCount = 0;
    glm::vec3 pen;

    const auto kTextHeight = lines.size() * kLineHeight;

    switch (textRenderer.vertAlignment) {
        case TextVertAlignment::TOP:
            pen.y = textRenderer.size.y;
            break;
        case TextVertAlignment::MIDDLE:
            pen.y = (textRenderer.size.y / 2) + (kTextHeight / 2);
            break;
        case TextVertAlignment::BOTTOM:
            pen.y = kTextHeight;
            break;
    }
    pen.y -= (textRenderer.size.y / 2) + kLineHeight;
    pen.y /= pixelsPerUnit;

    for (const auto& line: lines) {
        switch (textRenderer.horiAlignment) {
            case TextHoriAlignment::LEFT:
                pen.x = 0;
                break;
            case TextHoriAlignment::MIDDLE:
                pen.x = (textRenderer.size.x - line.width) / 2;
                break;
            case TextHoriAlignment::RIGHT:
                pen.x = textRenderer.size.x - line.width;
                break;
        }
        pen.x -= textRenderer.size.x / 2;
        pen.x /= pixelsPerUnit;

        for (const auto& word: line.words) {
            for (auto chr: word) {
                Glyph glyph;
                atlas->find_glyph(chr, glyph, textRenderer.fontSize);

                const auto bearing = glm::vec2(glyph.metrics.bearing) / pixelsPerUnit;
                const auto size = glm::vec2(glyph.metrics.size) / pixelsPerUnit;
                const auto advance = glyph.metrics.advance / pixelsPerUnit;

                glm::vec2 min = glm::vec2(pen.x + bearing.x, pen.y + bearing.y - size.y);// bottom left
                glm::vec2 max = glm::vec2(min.x + size.x, min.y + size.y);               // upper right

                if (min == max) {
                    pen.x += advance;
                    continue;//empty character, skip
                }

                // write positions
                glm::vec3 positions[4];
                positions[0] = glm::vec3(min.x, min.y, 0);
                positions[1] = glm::vec3(max.x, min.y, 0);
                positions[2] = glm::vec3(min.x, max.y, 0);
                positions[3] = glm::vec3(max.x, max.y, 0);
                m_positionBuffer->write(positions, 4 * sizeof(glm::vec3), m_textCount * sizeof(glm::vec3) * 4);

                // write coords
                glm::vec2 texCoords[4];
                texCoords[0] = glyph.uvMin;
                texCoords[1] = glm::vec2(glyph.uvMax.x, glyph.uvMin.y);
                texCoords[2] = glm::vec2(glyph.uvMin.x, glyph.uvMax.y);
                texCoords[3] = glyph.uvMax;
                m_texCoordBuffer->write(texCoords, 4 * sizeof(glm::vec2), m_textCount * sizeof(glm::vec2) * 4);

                // write indices
                uint32_t baseIndex = m_textCount * 4;
                uint32_t indices[6];
                indices[0] = baseIndex;
                indices[1] = baseIndex + 2;
                indices[2] = baseIndex + 1;
                indices[3] = baseIndex + 1;
                indices[4] = baseIndex + 2;
                indices[5] = baseIndex + 3;
                m_indexBuffer->write(indices, 6 * sizeof(uint32_t), m_textCount * 6 * sizeof(uint32_t));

                pen.x += advance;
                m_textCount++;
            }
        }
        // move pen to next line
        pen.y -= kLineHeight;
    }
    m_positionBuffer->flush();
    m_texCoordBuffer->flush();
    m_indexBuffer->flush();
}

void TextMesh::draw(duk::rhi::CommandBuffer* commandBuffer) {
    const duk::rhi::Buffer* buffers[] = {m_positionBuffer.get(), m_texCoordBuffer.get()};

    commandBuffer->bind_vertex_buffer(buffers, std::size(buffers), 0);

    commandBuffer->bind_index_buffer(m_indexBuffer.get());

    commandBuffer->draw_indexed(m_textCount * 6, 1, 0, 0, 0);
}

void TextMesh::reserve(uint32_t textCount) {
    const uint32_t kVertexCount = textCount * 4;
    const uint32_t kIndexCount = textCount * 6;
    {
        auto positionBuffer = detail::create_buffer(m_renderer, kVertexCount, sizeof(glm::vec3), duk::rhi::Buffer::Type::VERTEX);
        if (m_positionBuffer) {
            positionBuffer->copy_from(m_positionBuffer.get(), m_textCapacity * 4 * sizeof(glm::vec3), 0, 0);
        }
        m_positionBuffer = positionBuffer;
    }
    {
        auto texCoordBuffer = detail::create_buffer(m_renderer, kVertexCount, sizeof(glm::vec2), duk::rhi::Buffer::Type::VERTEX);
        if (m_texCoordBuffer) {
            texCoordBuffer->copy_from(m_texCoordBuffer.get(), m_textCapacity * 4 * sizeof(glm::vec2), 0, 0);
        }
        m_texCoordBuffer = texCoordBuffer;
    }
    {
        auto indexBuffer = detail::create_buffer(m_renderer, kIndexCount, sizeof(uint32_t), duk::rhi::Buffer::Type::INDEX_32);
        if (m_indexBuffer) {
            indexBuffer->copy_from(m_indexBuffer.get(), m_textCapacity * 6 * sizeof(uint32_t), 0, 0);
        }
        m_indexBuffer = indexBuffer;
    }
    m_textCapacity = textCount;
}

void update_text_renderer(Renderer* renderer, const objects::Component<TextRenderer>& textRenderer) {
    Font::BuildAtlasParams buildAtlasParams = {};
    buildAtlasParams.rhi = renderer->rhi();
    buildAtlasParams.commandQueue = renderer->main_command_queue();
    buildAtlasParams.fontSize = textRenderer->fontSize;

    auto atlas = textRenderer->font->atlas(buildAtlasParams);

    auto& material = textRenderer->material;
    auto& mesh = textRenderer->mesh;

    if (!material) {

        MaterialCreateInfo materialCreateInfo = {};
        materialCreateInfo.renderer = renderer;

        material = std::make_shared<Material>(materialCreateInfo);
        // auto pipeline = material->pipeline();
        // pipeline->blend(true);
        // pipeline->cull_mode_mask(duk::rhi::GraphicsPipeline::CullMode::BACK);
        // pipeline->depth_test(false);
    }

    if (!mesh) {
        TextMeshCreateInfo textMeshCreateInfo = {};
        textMeshCreateInfo.renderer = renderer;
        mesh = std::make_shared<TextMesh>(textMeshCreateInfo);
    }

    const auto transform = textRenderer.object().component<Transform>();
    const auto canvasTransform = textRenderer.object().component<CanvasTransform>();

    if (transform) {
        material->set("uInstance", transform->model);
        mesh->update_text(atlas, *textRenderer, 100);
    } else if (canvasTransform) {
        material->set("uInstance", canvasTransform->model);
        mesh->update_text(atlas, *textRenderer, 1);
    } else {
        material->set("uInstance", glm::mat4(1));
        mesh->update_text(atlas, *textRenderer, 1);
    }
}

}// namespace duk::renderer
