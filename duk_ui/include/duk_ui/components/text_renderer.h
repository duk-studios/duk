//
// Created by Ricardo on 30/03/2024.
//

#ifndef DUK_UI_TEXT_RENDERER_H
#define DUK_UI_TEXT_RENDERER_H

#include <duk_ui/font/font.h>
#include <duk_ui/text/text_alignment.h>
#include <duk_ui/text/text_mesh.h>
#include <duk_renderer/material/material.h>

namespace duk::ui {

struct TextRenderer;

struct TextRenderer {
    FontResource font;
    uint32_t fontSize;
    std::string text;
    glm::vec2 size;
    TextHoriAlignment horiAlignment;
    TextVertAlignment vertAlignment;
    bool dynamic;

    std::shared_ptr<TextMesh> mesh;
};

void update_text_renderer(duk::tools::Globals* globals, const duk::objects::Component<TextRenderer>& textRenderer);

}// namespace duk::ui

namespace duk::serial {

template<>
inline void from_json<duk::ui::TextRenderer>(const rapidjson::Value& json, duk::ui::TextRenderer& textRenderer) {
    from_json_member(json, "font", textRenderer.font);
    from_json_member(json, "fontSize", textRenderer.fontSize);
    from_json_member(json, "text", textRenderer.text);
    from_json_member(json, "size", textRenderer.size);
    from_json_member(json, "horiAlignment", textRenderer.horiAlignment);
    from_json_member(json, "vertAlignment", textRenderer.vertAlignment);
    from_json_member(json, "dynamic", textRenderer.dynamic);
}

template<>
inline void to_json<duk::ui::TextRenderer>(rapidjson::Document& document, rapidjson::Value& json, const duk::ui::TextRenderer& textRenderer) {
    to_json_member(document, json, "font", textRenderer.font);
    to_json_member(document, json, "fontSize", textRenderer.fontSize);
    to_json_member(document, json, "text", textRenderer.text);
    to_json_member(document, json, "size", textRenderer.size);
    to_json_member(document, json, "horiAlignment", textRenderer.horiAlignment);
    to_json_member(document, json, "vertAlignment", textRenderer.vertAlignment);
    to_json_member(document, json, "dynamic", textRenderer.dynamic);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::ui::TextRenderer& textRenderer) {
    solver->solve(textRenderer.font);
}

}// namespace duk::resource

#endif//DUK_UI_TEXT_RENDERER_H
