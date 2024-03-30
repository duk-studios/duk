//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_RENDERER_FREETYPE_IMPORT_H
#define DUK_RENDERER_FREETYPE_IMPORT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace duk::renderer {

extern std::string build_freetype_error_message(const char* format, FT_Error error);

}

#endif //DUK_RENDERER_FREETYPE_IMPORT_H
