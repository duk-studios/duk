//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/rhi_exception.h>

namespace duk::rhi {

RHIException::RHIException(RHIException::Type type, const std::string& description)
    : m_type(type)
    , m_description(description) {
}

RHIException::Type RHIException::type() const {
    return m_type;
}

const std::string& RHIException::description() const {
    return m_description;
}
}// namespace duk::rhi
