//
// Created by Ricardo on 30/03/2024.
//

#ifndef DUK_RENDERER_TEXT_RENDERER_H
#define DUK_RENDERER_TEXT_RENDERER_H

#include <duk_renderer/font/font.h>
#include <duk_renderer/material/material.h>

namespace duk::renderer {

struct TextRenderer;

struct TextMeshCreateInfo {
    Renderer* renderer;
};

class TextMesh {
public:
    TextMesh(const TextMeshCreateInfo& textBrushCreateInfo);

    void update_text(const FontAtlas* atlas, const TextRenderer& textRenderer, float pixelsPerUnit);

    void draw(duk::rhi::CommandBuffer* commandBuffer);

private:
    void reserve(uint32_t textCount);

private:
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Buffer> m_positionBuffer;
    std::shared_ptr<duk::rhi::Buffer> m_texCoordBuffer;
    std::shared_ptr<duk::rhi::Buffer> m_indexBuffer;
    uint32_t m_textCount;
    uint32_t m_textCapacity;
    duk::hash::Hash m_hash;
};

enum class TextHoriAlignment {
    LEFT,
    MIDDLE,
    RIGHT
};

enum class TextVertAlignment {
    TOP,
    MIDDLE,
    BOTTOM
};

struct TextRenderer {
    FontResource font;
    uint32_t fontSize;
    std::string text;
    glm::vec2 size;
    TextHoriAlignment horiAlignment;
    TextVertAlignment vertAlignment;
    std::shared_ptr<TextMesh> mesh;
    std::shared_ptr<Material> material;
};

void update_text_renderer(Renderer* renderer, const duk::objects::Component<TextRenderer>& textRenderer);

}// namespace duk::renderer

namespace duk::serial {

namespace detail {

static inline duk::renderer::TextHoriAlignment parse_text_hori_alignment(const char* str) {
    if (strcmp(str, "left") == 0) {
        return renderer::TextHoriAlignment::LEFT;
    }
    if (strcmp(str, "middle") == 0) {
        return renderer::TextHoriAlignment::MIDDLE;
    }
    if (strcmp(str, "right") == 0) {
        return renderer::TextHoriAlignment::RIGHT;
    }
    throw std::invalid_argument(fmt::format("invalid text horizontal alignment value: {}", str));
}

static inline std::string to_string(duk::renderer::TextHoriAlignment textAlignment) {
    switch (textAlignment) {
        case renderer::TextHoriAlignment::LEFT:
            return "left";
        case renderer::TextHoriAlignment::MIDDLE:
            return "middle";
        case renderer::TextHoriAlignment::RIGHT:
            return "right";
        default:
            throw std::invalid_argument(fmt::format("invalid TextHoriAlignment value: {}", (int)textAlignment));
    }
}

static inline duk::renderer::TextVertAlignment parse_text_vert_alignment(const char* str) {
    if (strcmp(str, "top") == 0) {
        return renderer::TextVertAlignment::TOP;
    }
    if (strcmp(str, "middle") == 0) {
        return renderer::TextVertAlignment::MIDDLE;
    }
    if (strcmp(str, "bottom") == 0) {
        return renderer::TextVertAlignment::BOTTOM;
    }
    throw std::invalid_argument(fmt::format("invalid text vertical alignment value: {}", str));
}

static inline std::string to_string(duk::renderer::TextVertAlignment textAlignment) {
    switch (textAlignment) {
        case renderer::TextVertAlignment::TOP:
            return "top";
        case renderer::TextVertAlignment::MIDDLE:
            return "middle";
        case renderer::TextVertAlignment::BOTTOM:
            return "bottom";
        default:
            throw std::invalid_argument(fmt::format("invalid TextVertAlignment value: {}", (int)textAlignment));
    }
}

}// namespace detail

template<>
inline void from_json<duk::renderer::TextHoriAlignment>(const rapidjson::Value& jsonObject, duk::renderer::TextHoriAlignment& textAlignment) {
    DUK_ASSERT(jsonObject.IsString());
    textAlignment = detail::parse_text_hori_alignment(jsonObject.GetString());
}

template<>
inline void to_json<duk::renderer::TextHoriAlignment>(const duk::renderer::TextHoriAlignment& textAlignment, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto str = detail::to_string(textAlignment);
    jsonObject.SetString(str.c_str(), str.size(), allocator);
}

template<>
inline void from_json<duk::renderer::TextVertAlignment>(const rapidjson::Value& jsonObject, duk::renderer::TextVertAlignment& textAlignment) {
    DUK_ASSERT(jsonObject.IsString());
    textAlignment = detail::parse_text_vert_alignment(jsonObject.GetString());
}

template<>
inline void to_json<duk::renderer::TextVertAlignment>(const duk::renderer::TextVertAlignment& textAlignment, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
    auto str = detail::to_string(textAlignment);
    jsonObject.SetString(str.c_str(), str.size(), allocator);
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::TextRenderer& textRenderer) {
    visitor->template visit_member<duk::resource::Resource>(textRenderer.font, MemberDescription("font"));
    visitor->visit_member(textRenderer.fontSize, MemberDescription("fontSize"));
    visitor->visit_member(textRenderer.text, MemberDescription("text"));
    visitor->visit_member(textRenderer.size, MemberDescription("size"));
    visitor->visit_member(textRenderer.horiAlignment, MemberDescription("horiAlignment"));
    visitor->visit_member(textRenderer.vertAlignment, MemberDescription("vertAlignment"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::TextRenderer& textRenderer) {
    solver->solve(textRenderer.font);
}

}// namespace duk::resource

#endif//DUK_RENDERER_TEXT_RENDERER_H
