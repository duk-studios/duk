//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_PLATFORM_H
#define DUK_PLATFORM_PLATFORM_H

#include <duk_platform/cursor.h>
#include <duk_platform/console.h>
#include <duk_platform/window.h>

namespace duk::platform {

class Platform {
public:
    virtual ~Platform();

    virtual Cursor* cursor() = 0;

    virtual Console* console() = 0;

    virtual std::shared_ptr<Window> create_window(const WindowCreateInfo& windowCreateInfo) = 0;

    virtual void pool_events() = 0;

    virtual void wait_events() = 0;
};

// creates the default platform object for this platform
extern std::unique_ptr<Platform> create_default_platform();

}// namespace duk::platform

#endif//DUK_PLATFORM_PLATFORM_H
