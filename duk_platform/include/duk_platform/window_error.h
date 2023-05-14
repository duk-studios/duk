//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_PLATFORM_WINDOW_ERROR_H
#define DUK_PLATFORM_WINDOW_ERROR_H

#include <duk_macros/macros.h>

#include <string>

namespace duk::platform {

class WindowError {
public:
    enum class Type {
        INTERNAL_ERROR = 0
    };

    WindowError(Type type, const std::string& description);

    DUK_NO_DISCARD Type type() const;

    DUK_NO_DISCARD const std::string& description() const;

private:
    Type m_type;
    std::string m_description;

};

}

#endif //DUK_PLATFORM_WINDOW_ERROR_H
