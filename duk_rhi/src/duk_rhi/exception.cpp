//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/exception.h>

namespace duk::rhi {

Exception::Exception(Exception::Type type, const std::string& description)
    : m_type(type)
    , m_description(description) {
}

Exception::Type Exception::type() const {
    return m_type;
}

const std::string& Exception::description() const {
    return m_description;
}
}// namespace duk::rhi
