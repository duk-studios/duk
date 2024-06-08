//
// Created by Ricardo on 07/06/2024.
//

#ifndef DUK_RENDERER_TEXT_ALIGNMENT_H
#define DUK_RENDERER_TEXT_ALIGNMENT_H

namespace duk::renderer {

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

}// namespace duk::renderer

namespace duk::serial {

namespace detail {

inline duk::renderer::TextHoriAlignment parse_text_hori_alignment(const char* str) {
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

inline std::string to_string(duk::renderer::TextHoriAlignment textAlignment) {
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

inline duk::renderer::TextVertAlignment parse_text_vert_alignment(const char* str) {
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

inline std::string to_string(duk::renderer::TextVertAlignment textAlignment) {
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
inline void to_json<duk::renderer::TextHoriAlignment>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::TextHoriAlignment& textAlignment) {
    auto str = detail::to_string(textAlignment);
    json.SetString(str.c_str(), str.size(), document.GetAllocator());
}

template<>
inline void from_json<duk::renderer::TextVertAlignment>(const rapidjson::Value& jsonObject, duk::renderer::TextVertAlignment& textAlignment) {
    DUK_ASSERT(jsonObject.IsString());
    textAlignment = detail::parse_text_vert_alignment(jsonObject.GetString());
}

template<>
inline void to_json<duk::renderer::TextVertAlignment>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::TextVertAlignment& textAlignment) {
    auto str = detail::to_string(textAlignment);
    json.SetString(str.c_str(), str.size(), document.GetAllocator());
}

}// namespace duk::serial

#endif//DUK_RENDERER_TEXT_ALIGNMENT_H
