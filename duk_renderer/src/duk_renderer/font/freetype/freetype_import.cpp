//
// Created by Ricardo on 29/03/2024.
//
#include <duk_renderer/font/freetype/freetype_import.h>

#include <fmt/format.h>

namespace duk::renderer {

std::string build_freetype_error_message(const char* format, FT_Error error) {
    return fmt::format("{0}. error: {1}", format, FT_Error_String(error));
}

}// namespace duk::renderer