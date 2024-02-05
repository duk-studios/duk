//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_ERROR_H
#define DUK_RHI_ERROR_H

#include <duk_macros/macros.h>

#include <string>

namespace duk::rhi {

class RHIError {
public:
    enum Type {
        INTERNAL_ERROR = 0,
        INVALID_ARGUMENT = 1,
        NOT_IMPLEMENTED = 2
    };
public:

    RHIError(Type type, const std::string& description);

    DUK_NO_DISCARD Type type() const;

    DUK_NO_DISCARD const std::string& description() const;

private:
    Type m_type;
    std::string m_description;
};

}

#endif //DUK_RHI_ERROR_H
