/// 24/02/2024
/// assert.h

#ifndef DUK_MACROS_ASSERT_H
#define DUK_MACROS_ASSERT_H

#include <duk_macros/macros.h>

namespace duk::macros {

void duk_assert(bool condition, const char* message, const char* file, int line);

}

#define DUK_ASSERT(x) duk::macros::duk_assert(x, DUK_STRINGIFY(x), __FILE__, __LINE__)

#endif// DUK_MACROS_ASSERT_H
