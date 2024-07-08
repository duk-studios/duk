/// 24/02/2024
/// assert.h

#ifndef DUK_MACROS_ASSERT_H
#define DUK_MACROS_ASSERT_H

#include <duk_macros/macros.h>

namespace duk::macros {

void duk_assert(bool condition, const char* message, const char* file, int line);

}

#ifdef DUK_DEBUG
#define DUK_ASSERT(x) duk::macros::duk_assert(x, DUK_STRINGIFY(x), __FILE__, __LINE__)
#else
#define DUK_ASSERT(x)
#endif

#endif// DUK_MACROS_ASSERT_H
