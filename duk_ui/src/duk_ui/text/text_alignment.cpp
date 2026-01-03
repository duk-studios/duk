//
// Created by rov on 03/01/2026.
//

#include <duk_ui/text/text_alignment.h>

using namespace duk::ui;
using namespace duk::serial;

namespace detail {

static TextHoriAlignment parse_text_hori_alignment(const char* str) {
    if (strcmp(str, "left") == 0) {
        return TextHoriAlignment::LEFT;
    }
    if (strcmp(str, "middle") == 0) {
        return TextHoriAlignment::MIDDLE;
    }
    if (strcmp(str, "right") == 0) {
        return TextHoriAlignment::RIGHT;
    }
    throw std::invalid_argument(fmt::format("invalid text horizontal alignment value: {}", str));
}

static std::string to_string(TextHoriAlignment textAlignment) {
    switch (textAlignment) {
        case TextHoriAlignment::LEFT:
            return "left";
        case TextHoriAlignment::MIDDLE:
            return "middle";
        case TextHoriAlignment::RIGHT:
            return "right";
        default:
            throw std::invalid_argument(fmt::format("invalid TextHoriAlignment value: {}", (int)textAlignment));
    }
}

static TextVertAlignment parse_text_vert_alignment(const char* str) {
    if (strcmp(str, "top") == 0) {
        return TextVertAlignment::TOP;
    }
    if (strcmp(str, "middle") == 0) {
        return TextVertAlignment::MIDDLE;
    }
    if (strcmp(str, "bottom") == 0) {
        return TextVertAlignment::BOTTOM;
    }
    throw std::invalid_argument(fmt::format("invalid text vertical alignment value: {}", str));
}

static std::string to_string(TextVertAlignment textAlignment) {
    switch (textAlignment) {
        case TextVertAlignment::TOP:
            return "top";
        case TextVertAlignment::MIDDLE:
            return "middle";
        case TextVertAlignment::BOTTOM:
            return "bottom";
        default:
            throw std::invalid_argument(fmt::format("invalid TextVertAlignment value: {}", (int)textAlignment));
    }
}

}// namespace detail

void JsonPrimitiveValue<TextVertAlignment>::write(rapidjson::Document& document, rapidjson::Value& json, const TextVertAlignment& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<TextVertAlignment>::read(const rapidjson::Value& json, TextVertAlignment& value) {
    DUK_ASSERT(json.IsString());
    value = detail::parse_text_vert_alignment(json.GetString());
}

void JsonPrimitiveValue<TextHoriAlignment>::write(rapidjson::Document& document, rapidjson::Value& json, const ui::TextHoriAlignment& value) {
    json.SetString(detail::to_string(value), document.GetAllocator());
}

void JsonPrimitiveValue<TextHoriAlignment>::read(const rapidjson::Value& json, ui::TextHoriAlignment& value) {
    DUK_ASSERT(json.IsString());
    value = detail::parse_text_hori_alignment(json.GetString());
}
