//
// Created by Ricardo on 24/06/2024.
//

#ifndef DUK_PLATFORM_CONSOLE_H
#define DUK_PLATFORM_CONSOLE_H

#include <duk_macros/macros.h>

#include <sstream>

namespace duk::platform {

class Console {
public:
    virtual ~Console();

    virtual bool attach() = 0;

    virtual bool open() = 0;

    virtual void close() = 0;

    virtual std::ostream& out() = 0;

    virtual std::ostream& err() = 0;

    virtual std::istream& in() = 0;

    DUK_NO_DISCARD virtual bool is_open() const = 0;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_CONSOLE_H
