//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_PLATFORM_H
#define DUK_PLATFORM_PLATFORM_H

#include <duk_platform/cursor.h>

namespace duk::platform {

class Platform {
public:
    static void create();

    virtual ~Platform();

    static Platform* instance();

    virtual Cursor* cursor() = 0;
};
}// namespace duk::platform

#endif//DUK_PLATFORM_PLATFORM_H
