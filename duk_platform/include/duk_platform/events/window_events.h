//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_WINDOW_EVENTS_H
#define DUK_WINDOW_EVENTS_H

namespace duk::platform::events {

struct WindowClose {

};

struct WindowDestroy {

};

struct WindowResize {
    uint32_t width;
    uint32_t height;
};

}


#endif //DUK_WINDOW_EVENTS_H
