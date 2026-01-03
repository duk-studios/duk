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

namespace duk::serial {

template<>
struct JsonPrimitiveValue<ui::TextVertAlignment> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const ui::TextVertAlignment& value);

    static void read(const rapidjson::Value& json, ui::TextVertAlignment& value);
};

template<>
struct JsonPrimitiveValue<ui::TextHoriAlignment> {
    static void write(rapidjson::Document& document, rapidjson::Value& json, const ui::TextHoriAlignment& value);

    static void read(const rapidjson::Value& json, ui::TextHoriAlignment& value);
};

}// namespace duk::serial

#endif//DUK_UI_TEXT_ALIGNMENT_H
