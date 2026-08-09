/// 11/11/2023
/// generator_utils.cpp

#include <duk_shader_generator/file_generators/generator_utils.h>

#include <cctype>
#include <fstream>
#include <stdexcept>
#include <unordered_map>

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

void write_file(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath);
    }
    file << content;
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

std::string binding_members_expression(const std::vector<rhi::BindingMemberDescription>& members) {
    std::ostringstream oss;
    oss << "{";
    for (const auto& member: members) {
        oss << "\n";
        oss << "                duk::rhi::BindingMemberDescription{"
            << member.offset << "u, " << member.size << "u, " << member.padding
            << "u, \"" << member.name << "\", \"" << member.typeName << "\"},";
    }
    if (!members.empty()) {
        oss << "\n";
    }
    oss << "}";
    return oss.str();
}

const std::string& glsl_to_cpp(const std::string& glslType) {
    static const std::unordered_map<std::string, std::string> kMapping = {
            {"bool",   "bool"},      {"int",    "int32_t"},  {"uint",   "uint32_t"},
            {"float",  "float"},     {"double", "double"},
            {"vec2",   "glm::vec2"}, {"vec3",   "glm::vec3"}, {"vec4",   "glm::vec4"},
            {"ivec2",  "glm::ivec2"},{"ivec3",  "glm::ivec3"},{"ivec4",  "glm::ivec4"},
            {"uvec2",  "glm::uvec2"},{"uvec3",  "glm::uvec3"},{"uvec4",  "glm::uvec4"},
            {"dvec2",  "glm::dvec2"},{"dvec3",  "glm::dvec3"},{"dvec4",  "glm::dvec4"},
            {"mat2",   "glm::mat2"}, {"mat3",   "glm::mat3"}, {"mat4",   "glm::mat4"},
            {"mat2x2", "glm::mat2x2"},{"mat2x3","glm::mat2x3"},{"mat2x4","glm::mat2x4"},
            {"mat3x2", "glm::mat3x2"},{"mat3x3","glm::mat3x3"},{"mat3x4","glm::mat3x4"},
            {"mat4x2", "glm::mat4x2"},{"mat4x3","glm::mat4x3"},{"mat4x4","glm::mat4x4"},
            {"dmat2",  "glm::dmat2"},{"dmat3",  "glm::dmat3"},{"dmat4",  "glm::dmat4"},
    };
    auto it = kMapping.find(glslType);
    return it != kMapping.end() ? it->second : glslType;
}

}// namespace duk::shader_generator::utils
