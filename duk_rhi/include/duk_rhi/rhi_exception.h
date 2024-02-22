//
// Created by Ricardo on 09/04/2023.
//

#ifndef DUK_RHI_EXCEPTION_H
#define DUK_RHI_EXCEPTION_H

#include <duk_macros/macros.h>

#include <string>

namespace duk::rhi {

class RHIException {
public:
    enum Type {
        INTERNAL_ERROR = 0,
        INVALID_ARGUMENT = 1,
        NOT_IMPLEMENTED = 2
    };

    RHIException(Type type, const std::string& description);

    DUK_NO_DISCARD Type type() const;

    DUK_NO_DISCARD const std::string& description() const;

private:
    Type m_type;
    std::string m_description;
};

}

#endif //DUK_RHI_EXCEPTION_H
