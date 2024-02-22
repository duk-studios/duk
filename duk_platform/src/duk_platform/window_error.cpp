//
// Created by Ricardo on 15/04/2023.
//

#include <duk_platform/window_error.h>

namespace duk::platform {

WindowError::WindowError(WindowError::Type type, const std::string& description)
    : m_type(type)
    , m_description(description) {
}

WindowError::Type WindowError::type() const {
    return m_type;
}

const std::string& WindowError::description() const {
    return m_description;
}

}// namespace duk::platform