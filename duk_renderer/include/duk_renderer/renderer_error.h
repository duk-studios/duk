//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RENDERER_RENDERER_ERROR_H
#define DUK_RENDERER_RENDERER_ERROR_H

#include <duk_macros/macros.h>

#include <string>

namespace duk::renderer {

class RendererError {
public:
    enum Type {
        INTERNAL_ERROR = 0,
        INVALID_ARGUMENT = 1,
        NOT_IMPLEMENTED = 2
    };
public:

    RendererError(Type type, const std::string& description);

    DUK_NO_DISCARD Type type() const;

    DUK_NO_DISCARD const std::string& description() const;

private:
    Type m_type;
    std::string m_description;
};

}

#endif //DUK_RENDERER_RENDERER_ERROR_H
