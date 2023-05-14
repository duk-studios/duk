/// 14/05/2023
/// assert.h

#ifndef DUK_MACROS_ASSERT_H
#define DUK_MACROS_ASSERT_H

#include <cassert>
#include <stdexcept>

#define DUK_ASSERT_MODE_ASSERT 0
#define DUK_ASSERT_MODE_EXCEPTION 1

#define DUK_ASSERT_EXCEPTION(expression, message) \
do { if (!(expression)) throw std::runtime_error(message); } while (false)

#if DUK_ASSERT_MODE == DUK_ASSERT_MODE_EXCEPTION
#define DUK_ASSERT(expression, message) DUK_ASSERT_EXCEPTION(expression, message)
#elif DUK_ASSERT_MODE == DUK_ASSERT_MODE_ASSERT
#define DUK_ASSERT(expression, message) assert(expression && message)
#else
#define DUK_ASSERT(expression, message)
#endif

#endif // DUK_MACROS_ASSERT_H

