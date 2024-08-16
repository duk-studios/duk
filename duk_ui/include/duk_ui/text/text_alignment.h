//
// Created by Ricardo on 07/06/2024.
//

#ifndef DUK_UI_TEXT_ALIGNMENT_H
#define DUK_UI_TEXT_ALIGNMENT_H

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

namespace detail {

inline duk::ui::TextHoriAlignment parse_text_hori_alignment(const char* str) {
    if (strcmp(str, "left") == 0) {
        return ui::TextHoriAlignment::LEFT;
    }
    if (strcmp(str, "middle") == 0) {
        return ui::TextHoriAlignment::MIDDLE;
    }
    if (strcmp(str, "right") == 0) {
        return ui::TextHoriAlignment::RIGHT;
    }
    throw std::invalid_argument(fmt::format("invalid text horizontal alignment value: {}", str));
}

inline std::string to_string(duk::ui::TextHoriAlignment textAlignment) {
    switch (textAlignment) {
        case ui::TextHoriAlignment::LEFT:
            return "left";
        case ui::TextHoriAlignment::MIDDLE:
            return "middle";
        case ui::TextHoriAlignment::RIGHT:
            return "right";
        default:
            throw std::invalid_argument(fmt::format("invalid TextHoriAlignment value: {}", (int)textAlignment));
    }
}

inline duk::ui::TextVertAlignment parse_text_vert_alignment(const char* str) {
    if (strcmp(str, "top") == 0) {
        return ui::TextVertAlignment::TOP;
    }
    if (strcmp(str, "middle") == 0) {
        return ui::TextVertAlignment::MIDDLE;
    }
    if (strcmp(str, "bottom") == 0) {
        return ui::TextVertAlignment::BOTTOM;
    }
    throw std::invalid_argument(fmt::format("invalid text vertical alignment value: {}", str));
}

inline std::string to_string(duk::ui::TextVertAlignment textAlignment) {
    switch (textAlignment) {
        case ui::TextVertAlignment::TOP:
            return "top";
        case ui::TextVertAlignment::MIDDLE:
            return "middle";
        case ui::TextVertAlignment::BOTTOM:
            return "bottom";
        default:
            throw std::invalid_argument(fmt::format("invalid TextVertAlignment value: {}", (int)textAlignment));
    }
}

}// namespace detail

template<>
inline void from_json<duk::ui::TextHoriAlignment>(const rapidjson::Value& jsonObject, duk::ui::TextHoriAlignment& textAlignment) {
    DUK_ASSERT(jsonObject.IsString());
    textAlignment = detail::parse_text_hori_alignment(jsonObject.GetString());
}

template<>
inline void to_json<duk::ui::TextHoriAlignment>(rapidjson::Document& document, rapidjson::Value& json, const duk::ui::TextHoriAlignment& textAlignment) {
    auto str = detail::to_string(textAlignment);
    json.SetString(str.c_str(), str.size(), document.GetAllocator());
}

template<>
inline void from_json<duk::ui::TextVertAlignment>(const rapidjson::Value& jsonObject, duk::ui::TextVertAlignment& textAlignment) {
    DUK_ASSERT(jsonObject.IsString());
    textAlignment = detail::parse_text_vert_alignment(jsonObject.GetString());
}

template<>
inline void to_json<duk::ui::TextVertAlignment>(rapidjson::Document& document, rapidjson::Value& json, const duk::ui::TextVertAlignment& textAlignment) {
    auto str = detail::to_string(textAlignment);
    json.SetString(str.c_str(), str.size(), document.GetAllocator());
}

}// namespace duk::serial

#endif//DUK_UI_TEXT_ALIGNMENT_H
