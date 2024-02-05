//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/rhi_error.h>

namespace duk::rhi {


RHIError::RHIError(RHIError::Type type, const std::string& description) :
    m_type(type),
    m_description(description) {

}

RHIError::Type RHIError::type() const {
    return m_type;
}

const std::string& RHIError::description() const {
    return m_description;
}
}
