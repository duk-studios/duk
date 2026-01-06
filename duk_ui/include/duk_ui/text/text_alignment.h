//
// Created by Ricardo on 07/06/2024.
//

#ifndef DUK_UI_TEXT_ALIGNMENT_H
#define DUK_UI_TEXT_ALIGNMENT_H

#include <duk_serial/json.h>

namespace duk::ui {

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

}// namespace duk::ui

namespace duk::type {
// clang-format off
template<>
struct Type<duk::ui::TextVertAlignment> : Enum<duk::ui::TextVertAlignment,
    Value<"top", duk::ui::TextVertAlignment::TOP>,
    Value<"middle", duk::ui::TextVertAlignment::MIDDLE>,
    Value<"bottom", duk::ui::TextVertAlignment::BOTTOM>> {
};

template<>
struct Type<duk::ui::TextHoriAlignment> : Enum<duk::ui::TextHoriAlignment,
    Value<"left", duk::ui::TextHoriAlignment::LEFT>,
    Value<"middle", duk::ui::TextHoriAlignment::MIDDLE>,
    Value<"right", duk::ui::TextHoriAlignment::RIGHT>> {

};

// clang-format on
}// namespace duk::type

#endif//DUK_UI_TEXT_ALIGNMENT_H
