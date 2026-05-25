/// generator_utils.h

#ifndef DUK_SHADER_GENERATOR_GENERATOR_UTILS_H
#define DUK_SHADER_GENERATOR_GENERATOR_UTILS_H

#include <duk_macros/macros.h>
#include <duk_rhi/shader.h>

#include <span>
#include <sstream>
#include <string>

namespace duk::shader_generator::utils {

// ---------------------------------------------------------------------------
// Code generation text utilities
// ---------------------------------------------------------------------------

void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName);

void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName);

void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes);

void generate_namespace_start(std::ostringstream& oss, const std::string& namespaceName);

void generate_namespace_end(std::ostringstream& oss, const std::string& namespaceName);

// ---------------------------------------------------------------------------
// RHI-specific expression utilities
// ---------------------------------------------------------------------------

DUK_NO_DISCARD std::string module_mask_expression(duk::rhi::ShaderModule::Mask mask);

// ---------------------------------------------------------------------------
// Generic iteration helper
// ---------------------------------------------------------------------------

template<typename T, typename P>
DUK_NO_DISCARD std::string generate_for_each(const T& container, P predicate, const char* separator = ",", bool onePerLine = true) {
    std::ostringstream oss;

    bool needsSeparator = false;
    for (const auto& element: container) {
        if (needsSeparator) {
            oss << separator;
            if (onePerLine) {
                oss << '\n';
            }
        }
        needsSeparator = true;
        oss << predicate(element);
    }

    return oss.str();
}

}// namespace duk::shader_generator::utils

#endif// DUK_SHADER_GENERATOR_GENERATOR_UTILS_H
