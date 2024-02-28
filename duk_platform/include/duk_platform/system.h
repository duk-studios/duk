//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_SYSTEM_H
#define DUK_PLATFORM_SYSTEM_H

#include <duk_platform/cursor.h>

namespace duk::platform {

class System {
public:
    static void create();

    virtual ~System() = default;

    static System* instance();

    virtual Cursor* cursor() = 0;

private:
    static System* s_instance;
};
}// namespace duk::platform

#endif//DUK_PLATFORM_SYSTEM_H
