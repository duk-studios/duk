//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_UI_FREETYPE_IMPORT_H
#define DUK_UI_FREETYPE_IMPORT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace duk::ui {

extern std::string build_freetype_error_message(const char* format, FT_Error error);

}

#endif//DUK_UI_FREETYPE_IMPORT_H
