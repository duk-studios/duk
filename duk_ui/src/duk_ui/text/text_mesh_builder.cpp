//
// Created by Ricardo on 08/06/2024.
//

#include <duk_ui/text/text_mesh_builder.h>

namespace duk::ui {

namespace detail {

static std::vector<std::string_view> collect_words(const std::string_view& text) {
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
    return words;
}

struct Line {
    int32_t width = 0;
    std::vector<std::string_view> words;
};

static std::vector<Line> collect_lines(const std::vector<std::string_view>& words, const FontAtlas* atlas, int32_t fontSize, const glm::vec2& textBoxSize, float lineHeight) {
    const auto kMaxLines = textBoxSize.y / lineHeight;

    std::vector<Line> lines;
    lines.reserve(kMaxLines);
    auto* currentLine = &lines.emplace_back();
    for (const auto& word: words) {
        int32_t wordWidth = 0;
        int32_t lastCharSpilling = 0;
        for (auto i = 0; i < word.size(); i++) {
            auto chr = word[i];

            Glyph glyph;
            atlas->find_glyph(chr, glyph, fontSize);

            wordWidth += glyph.metrics.advance;
            if (i == word.size() - 1) {
                // keep track of the last character spilling,
                // basically how much the advance goes out of the character bearing + size
                // the last character of the last word in a line must not use the complete advance size
                lastCharSpilling = glyph.metrics.advance - glyph.metrics.size.x - glyph.metrics.bearing.x;
            }
        }
        if ((currentLine->width + wordWidth) >= textBoxSize.x && !currentLine->words.empty()) {
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
    return lines;
}

static void update_mesh(duk::renderer::DynamicMesh* mesh, const std::vector<Line>& lines, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, float lineHeight, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment,
                        float pixelsPerUnit) {
    // build mesh by lines, taking alignment options into account
    glm::vec3 pen;

    const auto kTextHeight = lines.size() * lineHeight;

    switch (vertAlignment) {
        case TextVertAlignment::TOP:
            pen.y = textBoxSize.y;
            break;
        case TextVertAlignment::MIDDLE:
            pen.y = (textBoxSize.y / 2) + (kTextHeight / 2);
            break;
        case TextVertAlignment::BOTTOM:
            pen.y = kTextHeight;
            break;
    }
    pen.y -= lineHeight;
    pen.y /= pixelsPerUnit;

    uint32_t textCount = 0;

    mesh->clear();

    for (const auto& line: lines) {
        switch (horiAlignment) {
            case TextHoriAlignment::LEFT:
                pen.x = 0;
                break;
            case TextHoriAlignment::MIDDLE:
                pen.x = (textBoxSize.x - line.width) / 2;
                break;
            case TextHoriAlignment::RIGHT:
                pen.x = textBoxSize.x - line.width;
                break;
        }
        pen.x /= pixelsPerUnit;

        for (const auto& word: line.words) {
            for (auto chr: word) {
                Glyph glyph;
                atlas->find_glyph(chr, glyph, fontSize);

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
                mesh->write_vertices(duk::renderer::VertexAttributes::POSITION, positions);

                // write coords
                glm::vec2 texCoords[4];
                texCoords[0] = glyph.uvMin;
                texCoords[1] = glm::vec2(glyph.uvMax.x, glyph.uvMin.y);
                texCoords[2] = glm::vec2(glyph.uvMin.x, glyph.uvMax.y);
                texCoords[3] = glyph.uvMax;
                mesh->write_vertices(duk::renderer::VertexAttributes::UV, texCoords);

                // write indices
                uint32_t baseIndex = textCount * 4;
                uint32_t indices[6];
                indices[0] = baseIndex;
                indices[1] = baseIndex + 2;
                indices[2] = baseIndex + 1;
                indices[3] = baseIndex + 1;
                indices[4] = baseIndex + 2;
                indices[5] = baseIndex + 3;
                mesh->write_indices(indices);

                pen.x += advance;
                textCount++;
            }
        }
        // move pen to next line
        pen.y -= lineHeight;
    }
    mesh->flush();
}

}// namespace detail

std::shared_ptr<duk::renderer::DynamicMesh> allocate_text_mesh(duk::renderer::MeshBufferPool* meshBufferPool, uint32_t textCount, bool dynamic) {
    const auto vertexCount = textCount * 4;
    const auto indexCount = textCount * 6;

    duk::renderer::DynamicMeshCreateInfo dynamicMeshCreateInfo = {};
    dynamicMeshCreateInfo.meshBufferPool = meshBufferPool;
    dynamicMeshCreateInfo.attributes = duk::renderer::VertexAttributes({duk::renderer::VertexAttributes::POSITION, duk::renderer::VertexAttributes::UV});
    dynamicMeshCreateInfo.maxVertexCount = vertexCount;
    dynamicMeshCreateInfo.indexType = duk::rhi::IndexType::UINT32;
    dynamicMeshCreateInfo.maxIndexCount = indexCount;
    dynamicMeshCreateInfo.updateFrequency = dynamic ? duk::rhi::Buffer::UpdateFrequency::DYNAMIC : duk::rhi::Buffer::UpdateFrequency::STATIC;

    return std::make_shared<duk::renderer::DynamicMesh>(dynamicMeshCreateInfo);
}

void build_text_mesh(duk::renderer::DynamicMesh* mesh, const std::string_view& text, const FontAtlas* atlas, uint32_t fontSize, const glm::vec2& textBoxSize, TextVertAlignment vertAlignment, TextHoriAlignment horiAlignment, float pixelsPerUnit) {
    const float kLineHeight = fontSize + fontSize * (1.f / 4.f);
    // get words
    auto words = detail::collect_words(text);

    // separate them in lines
    auto lines = detail::collect_lines(words, atlas, fontSize, textBoxSize, kLineHeight);

    // update content
    detail::update_mesh(mesh, lines, atlas, fontSize, textBoxSize, kLineHeight, vertAlignment, horiAlignment, pixelsPerUnit);
}

}// namespace duk::ui