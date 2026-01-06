//
// Created by Ricardo on 30/03/2024.
//

#ifndef DUK_UI_TEXT_H
#define DUK_UI_TEXT_H

#include <duk_ui/font/font.h>
#include <duk_ui/text/text_alignment.h>
#include <duk_renderer/material/material.h>

namespace duk::ui {

struct Text {
    FontResource font;
    uint32_t fontSize;
    std::string text;
    glm::vec2 size;
    glm::vec4 color = glm::vec4(1);
    TextHoriAlignment horiAlignment;
    TextVertAlignment vertAlignment;
    bool dynamic;
    uint32_t capacity = 0;
    duk::hash::Hash hash = 0;
};

}// namespace duk::ui

namespace duk::type {
// clang-format off
template<>
struct Type<duk::ui::Text> : Class<duk::ui::Text,
    Member<"font", &duk::ui::Text::font>,
    Member<"fontSize", &duk::ui::Text::fontSize>,
    Member<"text", &duk::ui::Text::text>,
    Member<"size", &duk::ui::Text::size>,
    Member<"color", &duk::ui::Text::color>,
    Member<"horiAlignment", &duk::ui::Text::horiAlignment>,
    Member<"vertAlignment", &duk::ui::Text::vertAlignment>,
    Member<"dynamic", &duk::ui::Text::dynamic>> {
};

// clang-format on
}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::ui::Text& textRenderer) {
    solver->solve(textRenderer.font);
}

}// namespace duk::resource

#endif//DUK_UI_TEXT_H
