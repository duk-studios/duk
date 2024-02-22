//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_SYSTEM_H
#define DUK_SYSTEM_H

#include <duk_platform/cursor.h>

namespace duk::platform {

class System {
public:
    static void create();

    static System* instance();

    virtual Cursor* cursor() = 0;

private:
    static System* s_instance;
};
}



#endif //DUK_SYSTEM_H
