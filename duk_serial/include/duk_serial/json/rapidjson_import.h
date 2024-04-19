//
// Created by Ricardo on 18/04/2024.
//

#ifndef DUK_SERIAL_RAPIDJSON_IMPORT_H
#define DUK_SERIAL_RAPIDJSON_IMPORT_H

#include <duk_macros/assert.h>

#define RAPIDJSON_ASSERT(x) DUK_ASSERT(x)
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#endif//DUK_SERIAL_RAPIDJSON_IMPORT_H
