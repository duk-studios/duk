/// shader_data_source_generator.cpp

#include <duk_shader_generator/file_generators/shader_data_source_generator.h>
#include <duk_shader_generator/file_generators/generator_utils.h>
#include <duk_shader_generator/options.h>

#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_tools/string.h>

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <variant>

namespace duk::shader_generator {

namespace detail {

static std::string to_screaming_snake_case(const std::string& name) {
    std::string result;
    result.reserve(name.size() + 4);
    for (size_t i = 0; i < name.size(); i++) {
        const char c = name[i];
        if (std::isupper(static_cast<unsigned char>(c)) && i > 0 && name[i - 1] != '_') {
            result += '_';
        }
        result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

static const std::string& glsl_to_cpp(const std::string& typeName) {
    static const std::unordered_map<std::string, std::string> kMapping = {
            {"bool",   "bool"},
            {"int",    "int32_t"},
            {"uint",   "uint32_t"},
            {"float",  "float"},
            {"double", "double"},
            {"vec2",   "glm::vec2"},
            {"vec3",   "glm::vec3"},
            {"vec4",   "glm::vec4"},
            {"ivec2",  "glm::ivec2"},
            {"ivec3",  "glm::ivec3"},
            {"ivec4",  "glm::ivec4"},
            {"uvec2",  "glm::uvec2"},
            {"uvec3",  "glm::uvec3"},
            {"uvec4",  "glm::uvec4"},
            {"dvec2",  "glm::dvec2"},
            {"dvec3",  "glm::dvec3"},
            {"dvec4",  "glm::dvec4"},
            {"mat2",   "glm::mat2"},
            {"mat3",   "glm::mat3"},
            {"mat4",   "glm::mat4"},
            {"mat2x2", "glm::mat2x2"},
            {"mat2x3", "glm::mat2x3"},
            {"mat2x4", "glm::mat2x4"},
            {"mat3x2", "glm::mat3x2"},
            {"mat3x3", "glm::mat3x3"},
            {"mat3x4", "glm::mat3x4"},
            {"mat4x2", "glm::mat4x2"},
            {"mat4x3", "glm::mat4x3"},
            {"mat4x4", "glm::mat4x4"},
            {"dmat2",  "glm::dmat2"},
            {"dmat3",  "glm::dmat3"},
            {"dmat4",  "glm::dmat4"},
    };
    auto it = kMapping.find(typeName);
    return it != kMapping.end() ? it->second : typeName;
}

static const char* vertex_format_enumerator(duk::rhi::VertexInput::Format format) {
    switch (format) {
        case rhi::VertexInput::Format::INT8:    return "duk::rhi::VertexInput::Format::INT8";
        case rhi::VertexInput::Format::UINT8:   return "duk::rhi::VertexInput::Format::UINT8";
        case rhi::VertexInput::Format::INT16:   return "duk::rhi::VertexInput::Format::INT16";
        case rhi::VertexInput::Format::UINT16:  return "duk::rhi::VertexInput::Format::UINT16";
        case rhi::VertexInput::Format::INT32:   return "duk::rhi::VertexInput::Format::INT32";
        case rhi::VertexInput::Format::UINT32:  return "duk::rhi::VertexInput::Format::UINT32";
        case rhi::VertexInput::Format::FLOAT32: return "duk::rhi::VertexInput::Format::FLOAT32";
        case rhi::VertexInput::Format::VEC2:    return "duk::rhi::VertexInput::Format::VEC2";
        case rhi::VertexInput::Format::VEC3:    return "duk::rhi::VertexInput::Format::VEC3";
        case rhi::VertexInput::Format::VEC4:    return "duk::rhi::VertexInput::Format::VEC4";
        default:                                return "duk::rhi::VertexInput::Format::UNDEFINED";
    }
}

static const char* image_binding_type_enumerator(duk::rhi::ImageBindingType type) {
    switch (type) {
        case rhi::ImageBindingType::IMAGE:         return "duk::rhi::ImageBindingType::IMAGE";
        case rhi::ImageBindingType::IMAGE_SAMPLER: return "duk::rhi::ImageBindingType::IMAGE_SAMPLER";
        case rhi::ImageBindingType::STORAGE_IMAGE: return "duk::rhi::ImageBindingType::STORAGE_IMAGE";
        default:                                   return "duk::rhi::ImageBindingType::IMAGE";
    }
}

static const char* buffer_binding_type_enumerator(duk::rhi::BufferBindingType type) {
    switch (type) {
        case rhi::BufferBindingType::UNIFORM_BUFFER: return "duk::rhi::BufferBindingType::UNIFORM_BUFFER";
        case rhi::BufferBindingType::STORAGE_BUFFER: return "duk::rhi::BufferBindingType::STORAGE_BUFFER";
        default:                                     return "duk::rhi::BufferBindingType::UNIFORM_BUFFER";
    }
}

static const char* stage_enumerator(duk::rhi::ShaderModule::Bits stage) {
    switch (stage) {
        case rhi::ShaderModule::VERTEX:                  return "duk::rhi::ShaderModule::VERTEX";
        case rhi::ShaderModule::TESSELLATION_CONTROL:    return "duk::rhi::ShaderModule::TESSELLATION_CONTROL";
        case rhi::ShaderModule::TESSELLATION_EVALUATION: return "duk::rhi::ShaderModule::TESSELLATION_EVALUATION";
        case rhi::ShaderModule::GEOMETRY:                return "duk::rhi::ShaderModule::GEOMETRY";
        case rhi::ShaderModule::FRAGMENT:                return "duk::rhi::ShaderModule::FRAGMENT";
        case rhi::ShaderModule::COMPUTE:                 return "duk::rhi::ShaderModule::COMPUTE";
        default:                                         return "0";
    }
}

static const char* stage_spirv_var_name(duk::rhi::ShaderModule::Bits stage) {
    switch (stage) {
        case rhi::ShaderModule::VERTEX:                  return "kVertexSpirV";
        case rhi::ShaderModule::TESSELLATION_CONTROL:    return "kTessControlSpirV";
        case rhi::ShaderModule::TESSELLATION_EVALUATION: return "kTessEvalSpirV";
        case rhi::ShaderModule::GEOMETRY:                return "kGeometrySpirV";
        case rhi::ShaderModule::FRAGMENT:                return "kFragmentSpirV";
        case rhi::ShaderModule::COMPUTE:                 return "kComputeSpirV";
        default:                                         return "kUnknownSpirV";
    }
}

static std::string generate_spirv_array(const std::string& varName, const std::vector<uint8_t>& code) {
    std::ostringstream oss;
    oss << "static const std::array<uint8_t, " << code.size() << "> " << varName << " = {\n    ";
    for (size_t i = 0; i < code.size(); i++) {
        if (i > 0) {
            oss << ',';
            if (i % 24 == 0) {
                oss << "\n    ";
            }
        }
        oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(code[i]);
    }
    oss << std::dec << "\n};\n";
    return oss.str();
}

static bool has_buffer_members(const duk::rhi::BindingDescription& desc) {
    return std::holds_alternative<rhi::BufferBindingDescription>(desc.binding) &&
           !std::get<rhi::BufferBindingDescription>(desc.binding).members.empty();
}

static void write_file(const std::string& content, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("failed to write file at: " + path.string());
    }
    file << content;
}

// -----------------------------------------------------------------------
// Header content
// -----------------------------------------------------------------------

static std::string build_header_content(
        const std::string& fileName,
        const std::string& className,
        const std::string& outputNamespace,
        const duk::rhi::RuntimeShaderDataSource& source) {
    const std::string includes[] = {
            "duk_rhi/shader_data_source.h",
            "glm/glm.hpp",
            "cstdint",
            "unordered_map",
            "vector",
    };

    std::ostringstream oss;
    utils::generate_include_guard_start(oss, fileName);
    oss << '\n';
    utils::generate_include_directives(oss, includes);
    oss << '\n';
    utils::generate_namespace_start(oss, outputNamespace);
    oss << '\n';

    oss << "class " << className << " : public duk::rhi::ShaderDataSource {\n";
    oss << "public:\n";

    for (const auto& desc: source.binding_layout()) {
        if (!has_buffer_members(desc)) {
            continue;
        }
        const auto& bufDesc = std::get<rhi::BufferBindingDescription>(desc.binding);
        const auto structName = duk::tools::snake_to_pascal(desc.name) + "Data";
        oss << "    struct " << structName << " {\n";
        for (const auto& member: bufDesc.members) {
            oss << "        " << glsl_to_cpp(member.typeName) << " " << member.name << ";\n";
            if (member.padding > 0) {
                oss << "        uint8_t _padding_" << member.name << "[" << member.padding << "];\n";
            }
        }
        oss << "    };\n\n";
    }

    oss << "    enum class Binding : uint32_t {\n";
    for (uint32_t i = 0; i < static_cast<uint32_t>(source.binding_layout().size()); i++) {
        oss << "        " << to_screaming_snake_case(source.binding_layout()[i].name) << " = " << i << ",\n";
    }
    oss << "    };\n\n";

    oss << "    duk::rhi::ShaderModule::Mask module_mask() const override;\n\n";
    oss << "    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const override;\n\n";
    oss << "    const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& shader_modules() const override;\n\n";
    oss << "    const duk::rhi::ShaderBindingLayout& binding_layout() const override;\n\n";
    oss << "    const duk::rhi::VertexLayout& vertex_layout() const override;\n\n";
    oss << "protected:\n";
    oss << "    duk::hash::Hash calculate_hash() const override;\n";
    oss << "};\n";

    oss << '\n';
    utils::generate_namespace_end(oss, outputNamespace);
    oss << '\n';
    utils::generate_include_guard_end(oss, fileName);

    return oss.str();
}

// -----------------------------------------------------------------------
// Source content
// -----------------------------------------------------------------------

static std::string build_source_content(
        const std::string& headerIncludePath,
        const std::string& className,
        const std::string& outputNamespace,
        const duk::rhi::RuntimeShaderDataSource& source) {
    static constexpr rhi::ShaderModule::Bits kStageOrder[] = {
            rhi::ShaderModule::VERTEX,
            rhi::ShaderModule::TESSELLATION_CONTROL,
            rhi::ShaderModule::TESSELLATION_EVALUATION,
            rhi::ShaderModule::GEOMETRY,
            rhi::ShaderModule::FRAGMENT,
            rhi::ShaderModule::COMPUTE,
    };

    rhi::ShaderModule::Mask moduleMask = source.module_mask();

    const std::string includes[] = {
            headerIncludePath,
            "duk_rhi/shader.h",
            "duk_rhi/vertex_layout.h",
            "array",
            "unordered_map",
    };

    std::ostringstream oss;
    utils::generate_include_directives(oss, includes);
    oss << '\n';
    utils::generate_namespace_start(oss, outputNamespace);
    oss << '\n';

    oss << "namespace {\n\n";

    std::vector<std::pair<rhi::ShaderModule::Bits, std::string>> stageVarNames;
    for (auto stage: kStageOrder) {
        auto it = source.shader_modules().find(stage);
        if (it == source.shader_modules().end()) {
            continue;
        }
        const std::string varName = stage_spirv_var_name(stage);
        oss << generate_spirv_array(varName, it->second) << '\n';
        stageVarNames.emplace_back(stage, varName);
    }

    oss << "static const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> kShaderModules = [] {\n";
    oss << "    std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> m;\n";
    for (const auto& [stage, varName]: stageVarNames) {
        oss << "    m[" << stage_enumerator(stage) << "] = std::vector<uint8_t>(" << varName << ".begin(), " << varName << ".end());\n";
    }
    oss << "    return m;\n}();\n\n";

    oss << "static const duk::rhi::ShaderBindingLayout kBindingLayout = [] {\n";
    oss << "    duk::rhi::ShaderBindingLayout layout;\n";
    for (const auto& desc: source.binding_layout()) {
        oss << "    layout.push_back(duk::rhi::BindingDescription{\n";
        if (std::holds_alternative<rhi::ImageBindingDescription>(desc.binding)) {
            const auto& imgDesc = std::get<rhi::ImageBindingDescription>(desc.binding);
            oss << "        duk::rhi::ImageBindingDescription{" << image_binding_type_enumerator(imgDesc.type) << "},\n";
        } else {
            const auto& bufDesc = std::get<rhi::BufferBindingDescription>(desc.binding);
            oss << "        duk::rhi::BufferBindingDescription{\n";
            oss << "            " << buffer_binding_type_enumerator(bufDesc.type) << ",\n";
            oss << "            " << bufDesc.size << "u,\n";
            oss << "            " << bufDesc.stride << "u,\n";
            oss << "            {\n";
            for (const auto& member: bufDesc.members) {
                oss << "                duk::rhi::BufferMemberDescription{" << member.offset << "u, " << member.size << "u, " << member.padding << "u, \"" << member.name << "\", \"" << member.typeName << "\"},\n";
            }
            oss << "            }\n";
            oss << "        },\n";
        }
        oss << "        " << utils::module_mask_expression(desc.moduleMask) << ",\n";
        oss << "        \"" << desc.name << "\"\n";
        oss << "    });\n";
    }
    oss << "    return layout;\n}();\n\n";

    oss << "static const duk::rhi::VertexLayout kVertexLayout = {\n";
    for (auto fmt: source.vertex_layout()) {
        oss << "    " << vertex_format_enumerator(fmt) << ",\n";
    }
    oss << "};\n\n";

    oss << "}// namespace\n\n";

    oss << "duk::rhi::ShaderModule::Mask " << className << "::module_mask() const {\n";
    oss << "    return " << utils::module_mask_expression(moduleMask) << ";\n";
    oss << "}\n\n";

    oss << "const std::vector<uint8_t>& " << className << "::shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const {\n";
    oss << "    return kShaderModules.at(type);\n";
    oss << "}\n\n";

    oss << "const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& " << className << "::shader_modules() const {\n";
    oss << "    return kShaderModules;\n";
    oss << "}\n\n";

    oss << "const duk::rhi::ShaderBindingLayout& " << className << "::binding_layout() const {\n";
    oss << "    return kBindingLayout;\n";
    oss << "}\n\n";

    oss << "const duk::rhi::VertexLayout& " << className << "::vertex_layout() const {\n";
    oss << "    return kVertexLayout;\n";
    oss << "}\n\n";

    oss << "duk::hash::Hash " << className << "::calculate_hash() const {\n";
    oss << "    return static_cast<duk::hash::Hash>(" << source.hash() << "ULL);\n";
    oss << "}\n";

    oss << '\n';
    utils::generate_namespace_end(oss, outputNamespace);

    return oss.str();
}

}// namespace detail

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

GeneratedFiles generate_shader_data_source(
        const std::string& shaderName,
        const std::string& outputNamespace,
        const std::string& headerIncludeDirectory,
        const duk::rhi::RuntimeShaderDataSource& source) {
    const auto fileName = shaderName + "_shader_data_source";
    const auto className = duk::tools::snake_to_pascal(shaderName) + "ShaderDataSource";
    const auto headerIncludePath = headerIncludeDirectory + "/" + fileName + ".h";

    return GeneratedFiles{
            detail::build_header_content(fileName, className, outputNamespace, source),
            detail::build_source_content(headerIncludePath, className, outputNamespace, source),
    };
}

void write_shader_data_source(const Options& options, const duk::rhi::RuntimeShaderDataSource& source) {
    const auto& includeDir = options.headerIncludePrefix.empty() ? options.outputIncludeDirectory : options.headerIncludePrefix;
    const auto files = generate_shader_data_source(options.shaderName, options.outputNamespace, includeDir, source);
    const auto fileName = options.shaderName + "_shader_data_source";

    detail::write_file(files.headerContent, std::filesystem::path(options.outputIncludeDirectory) / (fileName + ".h"));
    detail::write_file(files.sourceContent, std::filesystem::path(options.outputSourceDirectory)  / (fileName + ".cpp"));
}

}// namespace duk::shader_generator

