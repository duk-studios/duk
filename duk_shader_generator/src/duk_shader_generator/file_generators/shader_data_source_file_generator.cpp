/// 04/11/2023
/// shader_data_source_file_generator.cpp

#include <duk_shader_generator/file_generators/shader_data_source_file_generator.h>
#include <duk_shader_generator/file_generators/generator_utils.h>

#include <duk_rhi/vertex_layout.h>
#include <duk_tools/string.h>

#include <cctype>
#include <filesystem>
#include <iomanip>
#include <map>
#include <sstream>
#include <unordered_map>
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

static const char* stage_spirv_var_name(rhi::ShaderModule::Bits stage) {
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

static const char* stage_enumerator(rhi::ShaderModule::Bits stage) {
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

static const char* image_binding_type_enumerator(rhi::ImageBindingType type) {
    switch (type) {
        case rhi::ImageBindingType::IMAGE:         return "duk::rhi::ImageBindingType::IMAGE";
        case rhi::ImageBindingType::IMAGE_SAMPLER: return "duk::rhi::ImageBindingType::IMAGE_SAMPLER";
        case rhi::ImageBindingType::STORAGE_IMAGE: return "duk::rhi::ImageBindingType::STORAGE_IMAGE";
        default:                                   return "duk::rhi::ImageBindingType::IMAGE";
    }
}

static const char* buffer_binding_type_enumerator(rhi::BufferBindingType type) {
    switch (type) {
        case rhi::BufferBindingType::UNIFORM_BUFFER: return "duk::rhi::BufferBindingType::UNIFORM_BUFFER";
        case rhi::BufferBindingType::STORAGE_BUFFER: return "duk::rhi::BufferBindingType::STORAGE_BUFFER";
        default:                                     return "duk::rhi::BufferBindingType::UNIFORM_BUFFER";
    }
}

static const char* vertex_format_enumerator(rhi::VertexInput::Format format) {
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

static constexpr rhi::ShaderModule::Bits kStageOrder[] = {
        rhi::ShaderModule::VERTEX, rhi::ShaderModule::TESSELLATION_CONTROL, rhi::ShaderModule::TESSELLATION_EVALUATION,
        rhi::ShaderModule::GEOMETRY, rhi::ShaderModule::FRAGMENT, rhi::ShaderModule::COMPUTE,
};

}// namespace detail

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

ShaderDataSourceFileGenerator::ShaderDataSourceFileGenerator(const Options& options, const rhi::RuntimeShaderDataSource& shaderDataSource)
    : m_options(options)
    , m_shaderDataSource(shaderDataSource) {
    m_fileName = options.outputShaderName + "_shader_data_source";
    m_className = duk::tools::snake_to_pascal(options.outputShaderName) + "ShaderDataSource";
    {
        std::ostringstream oss;
        generate_header(oss);
        const auto path = std::filesystem::path(options.outputIncludeDirectory) / (m_fileName + ".h");
        std::filesystem::create_directories(path.parent_path());
        write_file(oss.str(), path.string());
    }

    {
        std::ostringstream oss;
        generate_source(oss);
        const auto path = std::filesystem::path(options.outputSourceDirectory) / (m_fileName + ".cpp");
        std::filesystem::create_directories(path.parent_path());
        write_file(oss.str(), path.string());
    }
}

// -----------------------------------------------------------------------
// Shader data source header
// -----------------------------------------------------------------------

void ShaderDataSourceFileGenerator::generate_header(
        std::ostringstream& oss) const {
    const std::vector<std::string> includes = {
            "duk_rhi/shader_data_source.h", "glm/glm.hpp", "cstdint", "unordered_map", "vector"};

    generate_include_guard_start(oss, m_fileName);
    oss << '\n';
    generate_include_directives(oss, includes);
    oss << '\n';
    generate_namespace_start(oss, m_options.outputNamespace);
    oss << '\n';
    generate_class_declaration(oss);
    oss << '\n';
    generate_namespace_end(oss, m_options.outputNamespace);
    oss << '\n';
    generate_include_guard_end(oss, m_fileName);
}

void ShaderDataSourceFileGenerator::generate_class_declaration(
        std::ostringstream& oss) const {
    oss << "class " << m_className << " : public duk::rhi::ShaderDataSource {\n";
    oss << "public:\n";

    const auto& bindingLayout = m_shaderDataSource.binding_layout();

    oss << "    enum class Binding : uint32_t {\n";
    for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
        oss << "        " << detail::to_screaming_snake_case(bindingLayout[i].name) << " = " << i << ",\n";
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
}

// -----------------------------------------------------------------------
// Shader data source source
// -----------------------------------------------------------------------

void ShaderDataSourceFileGenerator::generate_source(std::ostringstream& oss) const {
    const std::vector<std::string> includes = {
            header_include_path(), "duk_rhi/shader.h", "duk_rhi/vertex_layout.h", "array", "unordered_map"};

    generate_include_directives(oss, includes);
    oss << '\n';
    generate_namespace_start(oss, m_options.outputNamespace);
    oss << '\n';
    generate_class_definition(oss);
    oss << '\n';
    generate_namespace_end(oss, m_options.outputNamespace);
}

void ShaderDataSourceFileGenerator::generate_class_definition(std::ostringstream& oss) const {
    rhi::ShaderModule::Mask moduleMask = m_shaderDataSource.module_mask();

    oss << "namespace {\n\n";

    const auto& modules = m_shaderDataSource.shader_modules();
    const auto& bindingLayout = m_shaderDataSource.binding_layout();
    const auto& vertexLayout = m_shaderDataSource.vertex_layout();

    std::vector<std::pair<rhi::ShaderModule::Bits, std::string>> stageVarNames;
    for (auto stage: detail::kStageOrder) {
        auto it = modules.find(stage);
        if (it == modules.end()) {
            continue;
        }
        const auto varName = std::string(detail::stage_spirv_var_name(stage));
        oss << detail::generate_spirv_array(varName, it->second) << '\n';
        stageVarNames.emplace_back(stage, varName);
    }

    oss << "static const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> kShaderModules = [] {\n";
    oss << "    std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> m;\n";
    for (const auto& [stage, varName]: stageVarNames) {
        oss << "    m[" << detail::stage_enumerator(stage)
            << "] = std::vector<uint8_t>(" << varName << ".begin(), " << varName << ".end());\n";
    }
    oss << "    return m;\n}();\n\n";

    oss << "static const duk::rhi::ShaderBindingLayout kBindingLayout = [] {\n";
    oss << "    duk::rhi::ShaderBindingLayout layout;\n";
    for (const auto& desc: bindingLayout) {
        oss << "    layout.push_back(duk::rhi::BindingDescription{\n";
        if (std::holds_alternative<rhi::ImageBindingDescription>(desc.binding)) {
            const auto& imgDesc = std::get<rhi::ImageBindingDescription>(desc.binding);
            oss << "        duk::rhi::ImageBindingDescription{"
                << detail::image_binding_type_enumerator(imgDesc.type) << "},\n";
        } else {
            const auto& bufDesc = std::get<rhi::BufferBindingDescription>(desc.binding);
            oss << "        duk::rhi::BufferBindingDescription{\n";
            oss << "            " << detail::buffer_binding_type_enumerator(bufDesc.type) << ",\n";
            oss << "            " << bufDesc.size << "u,\n";
            oss << "            " << bufDesc.stride << "u,\n";
            oss << "            {\n";
            for (const auto& member: bufDesc.members) {
                oss << "                duk::rhi::BufferMemberDescription{"
                    << member.offset << "u, " << member.size << "u, " << member.padding
                    << "u, \"" << member.name << "\", \"" << member.typeName << "\"},\n";
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
    for (auto fmt: vertexLayout) {
        oss << "    " << detail::vertex_format_enumerator(fmt) << ",\n";
    }
    oss << "};\n\n";
    oss << "}// namespace\n\n";

    oss << "duk::rhi::ShaderModule::Mask " << m_className << "::module_mask() const {\n";
    oss << "    return " << utils::module_mask_expression(moduleMask) << ";\n}\n\n";

    oss << "const std::vector<uint8_t>& " << m_className
        << "::shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const {\n";
    oss << "    return kShaderModules.at(type);\n}\n\n";

    oss << "const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& "
        << m_className << "::shader_modules() const {\n";
    oss << "    return kShaderModules;\n}\n\n";

    oss << "const duk::rhi::ShaderBindingLayout& " << m_className << "::binding_layout() const {\n";
    oss << "    return kBindingLayout;\n}\n\n";

    oss << "const duk::rhi::VertexLayout& " << m_className << "::vertex_layout() const {\n";
    oss << "    return kVertexLayout;\n}\n\n";

    oss << "duk::hash::Hash " << m_className << "::calculate_hash() const {\n";
    oss << "    return static_cast<duk::hash::Hash>(" << m_shaderDataSource.hash() << "ULL);\n}\n";
}

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

std::string ShaderDataSourceFileGenerator::header_include_path() const {
    const auto& prefix = m_options.outputHeaderIncludePrefix;
    if (!prefix.empty()) {
        return prefix + "/" + m_fileName + ".h";
    }
    return m_options.outputIncludeDirectory + "/" + m_fileName + ".h";
}

}// namespace duk::shader_generator
