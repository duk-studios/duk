/// 11/11/2023
/// generator_utils.cpp

#include <duk_shader_generator/file_generators/generator_utils.h>

#include <cctype>

namespace duk::shader_generator::utils {

namespace detail {

static std::string include_guard_name(const std::string& fileName) {
    std::string guard = "DUK_GENERATED_";
    for (char c: fileName) {
        guard += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    guard += "_H";
    return guard;
}

}// namespace detail

void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName) {
    const auto guard = detail::include_guard_name(fileName);
    oss << "#ifndef " << guard << '\n';
    oss << "#define " << guard << '\n';
}

void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName) {
    const auto guard = detail::include_guard_name(fileName);
    oss << "#endif// " << guard << '\n';
}

void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include: includes) {
        oss << "#include <" << include << ">\n";
    }
}

void generate_namespace_start(std::ostringstream& oss, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        return;
    }
    oss << "namespace " << namespaceName << " {\n";
}

void generate_namespace_end(std::ostringstream& oss, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        return;
    }
    oss << "}// namespace " << namespaceName << '\n';
}

std::string module_mask_expression(duk::rhi::ShaderModule::Mask mask) {
    static constexpr std::pair<duk::rhi::ShaderModule::Bits, const char*> kBits[] = {
            {rhi::ShaderModule::VERTEX,                  "duk::rhi::ShaderModule::VERTEX"},
            {rhi::ShaderModule::TESSELLATION_CONTROL,    "duk::rhi::ShaderModule::TESSELLATION_CONTROL"},
            {rhi::ShaderModule::TESSELLATION_EVALUATION, "duk::rhi::ShaderModule::TESSELLATION_EVALUATION"},
            {rhi::ShaderModule::GEOMETRY,                "duk::rhi::ShaderModule::GEOMETRY"},
            {rhi::ShaderModule::FRAGMENT,                "duk::rhi::ShaderModule::FRAGMENT"},
            {rhi::ShaderModule::COMPUTE,                 "duk::rhi::ShaderModule::COMPUTE"},
    };

    std::ostringstream oss;
    bool first = true;
    for (const auto& [bit, name]: kBits) {
        if (mask & bit) {
            if (!first) {
                oss << " | ";
            }
            oss << name;
            first = false;
        }
    }

    if (first) {
        oss << '0';
    }

    return oss.str();
}

}// namespace duk::shader_generator::utils
