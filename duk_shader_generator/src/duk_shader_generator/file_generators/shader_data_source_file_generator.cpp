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

static const char* binding_type_enumerator(rhi::BindingType type) {
    switch (type) {
        case rhi::BindingType::IMAGE:          return "duk::rhi::BindingType::IMAGE";
        case rhi::BindingType::SAMPLER_IMAGE:  return "duk::rhi::BindingType::SAMPLER_IMAGE";
        case rhi::BindingType::STORAGE_IMAGE:  return "duk::rhi::BindingType::STORAGE_IMAGE";
        case rhi::BindingType::UNIFORM_BUFFER: return "duk::rhi::BindingType::UNIFORM_BUFFER";
        case rhi::BindingType::STORAGE_BUFFER: return "duk::rhi::BindingType::STORAGE_BUFFER";
        default:                               throw std::runtime_error("Unknown image binding type");
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

static std::string header_include_path(const Options& options, const std::string& fileName) {
    const auto& prefix = options.outputHeaderIncludePrefix;
    if (!prefix.empty()) {
        return prefix + "/" + fileName + ".h";
    }
    return options.outputIncludeDirectory + "/" + fileName + ".h";
}

// -----------------------------------------------------------------------
// Shader data source header
// -----------------------------------------------------------------------

static void generate_class_declaration(
        std::ostringstream& oss, const std::string& className, const rhi::RuntimeShaderDataSource& shaderDataSource) {
    oss << "class " << className << " : public duk::rhi::ShaderDataSource {\n";
    oss << "public:\n";

    const auto& bindingLayout = shaderDataSource.binding_layout();

    oss << "    enum class Binding : uint32_t {\n";
    for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
        oss << "        " << to_screaming_snake_case(bindingLayout[i].name) << " = " << i << ",\n";
    }
    oss << "    };\n\n";

    oss << "    duk::rhi::ShaderModule::Mask module_mask() const override;\n\n";
    oss << "    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const override;\n\n";
    oss << "    const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& shader_modules() const override;\n\n";
    oss << "    const duk::rhi::BindingLayout& binding_layout() const override;\n\n";
    oss << "    const duk::rhi::VertexLayout& vertex_layout() const override;\n\n";
    oss << "protected:\n";
    oss << "    duk::hash::Hash calculate_hash() const override;\n";
    oss << "};\n";
}

static void generate_header(
        std::ostringstream& oss,
        const Options& options,
        const std::string& fileName,
        const std::string& className,
        const rhi::RuntimeShaderDataSource& shaderDataSource) {
    const std::vector<std::string> includes = {
            "duk_rhi/shader_data_source.h", "glm/glm.hpp", "cstdint", "unordered_map", "vector"};

    utils::generate_include_guard_start(oss, fileName);
    oss << '\n';
    utils::generate_include_directives(oss, includes);
    oss << '\n';
    utils::generate_namespace_start(oss, options.outputNamespace);
    oss << '\n';
    generate_class_declaration(oss, className, shaderDataSource);
    oss << '\n';
    utils::generate_namespace_end(oss, options.outputNamespace);
    oss << '\n';
    utils::generate_include_guard_end(oss, fileName);
}

// -----------------------------------------------------------------------
// Shader data source source
// -----------------------------------------------------------------------

static void generate_class_definition(
        std::ostringstream& oss, const std::string& className, const rhi::RuntimeShaderDataSource& shaderDataSource) {
    rhi::ShaderModule::Mask moduleMask = shaderDataSource.module_mask();

    oss << "namespace {\n\n";

    const auto& modules = shaderDataSource.shader_modules();
    const auto& bindingLayout = shaderDataSource.binding_layout();
    const auto& vertexLayout = shaderDataSource.vertex_layout();

    std::vector<std::pair<rhi::ShaderModule::Bits, std::string>> stageVarNames;
    for (auto stage: kStageOrder) {
        auto it = modules.find(stage);
        if (it == modules.end()) {
            continue;
        }
        const auto varName = std::string(stage_spirv_var_name(stage));
        oss << generate_spirv_array(varName, it->second) << '\n';
        stageVarNames.emplace_back(stage, varName);
    }

    oss << "static const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> kShaderModules = [] {\n";
    oss << "    std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>> m;\n";
    for (const auto& [stage, varName]: stageVarNames) {
        oss << "    m[" << stage_enumerator(stage)
            << "] = std::vector<uint8_t>(" << varName << ".begin(), " << varName << ".end());\n";
    }
    oss << "    return m;\n}();\n\n";

    oss << "static const duk::rhi::BindingLayout kBindingLayout = [] {\n";
    oss << "    duk::rhi::BindingLayout layout;\n";
    for (const auto& desc: bindingLayout) {
        const auto bindingType = binding_type_enumerator(desc.type);
        oss << "    layout.push_back(duk::rhi::BindingDescription{\n";
        oss << "            .type = " << bindingType << ",\n";
        oss << "            .moduleMask = " << utils::module_mask_expression(desc.moduleMask) << ",\n";
        oss << "            .name = \"" << desc.name << "\",\n";
        oss << "            .size = " << desc.size << ",\n";
        oss << "            .stride = " << desc.stride << ",\n";
        oss << "            .members = " << utils::binding_members_expression(desc.members) << "\n";
        oss << "    });\n";
    }
    oss << "    return layout;\n}();\n\n";

    oss << "static const duk::rhi::VertexLayout kVertexLayout = {\n";
    for (auto fmt: vertexLayout) {
        oss << "    " << vertex_format_enumerator(fmt) << ",\n";
    }
    oss << "};\n\n";
    oss << "}// namespace\n\n";

    oss << "duk::rhi::ShaderModule::Mask " << className << "::module_mask() const {\n";
    oss << "    return " << utils::module_mask_expression(moduleMask) << ";\n}\n\n";

    oss << "const std::vector<uint8_t>& " << className
        << "::shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const {\n";
    oss << "    return kShaderModules.at(type);\n}\n\n";

    oss << "const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& "
        << className << "::shader_modules() const {\n";
    oss << "    return kShaderModules;\n}\n\n";

    oss << "const duk::rhi::BindingLayout& " << className << "::binding_layout() const {\n";
    oss << "    return kBindingLayout;\n}\n\n";

    oss << "const duk::rhi::VertexLayout& " << className << "::vertex_layout() const {\n";
    oss << "    return kVertexLayout;\n}\n\n";

    oss << "duk::hash::Hash " << className << "::calculate_hash() const {\n";
    oss << "    return static_cast<duk::hash::Hash>(" << shaderDataSource.hash() << "ULL);\n}\n";
}

static void generate_source(
        std::ostringstream& oss,
        const Options& options,
        const std::string& fileName,
        const std::string& className,
        const rhi::RuntimeShaderDataSource& shaderDataSource) {
    const std::vector<std::string> includes = {
            header_include_path(options, fileName), "duk_rhi/shader.h", "duk_rhi/vertex_layout.h", "array", "unordered_map"};

    utils::generate_include_directives(oss, includes);
    oss << '\n';
    utils::generate_namespace_start(oss, options.outputNamespace);
    oss << '\n';
    generate_class_definition(oss, className, shaderDataSource);
    oss << '\n';
    utils::generate_namespace_end(oss, options.outputNamespace);
}

}// namespace detail

void generate_shader_data_source_files(const Options& options, const rhi::RuntimeShaderDataSource& shaderDataSource) {
    const auto fileName = options.outputShaderName + "_shader_data_source";
    const auto className = duk::tools::snake_to_pascal(options.outputShaderName) + "ShaderDataSource";

    {
        std::ostringstream oss;
        detail::generate_header(oss, options, fileName, className, shaderDataSource);
        const auto path = std::filesystem::path(options.outputIncludeDirectory) / (fileName + ".h");
        std::filesystem::create_directories(path.parent_path());
        utils::write_file(oss.str(), path.string());
    }

    {
        std::ostringstream oss;
        detail::generate_source(oss, options, fileName, className, shaderDataSource);
        const auto path = std::filesystem::path(options.outputSourceDirectory) / (fileName + ".cpp");
        std::filesystem::create_directories(path.parent_path());
        utils::write_file(oss.str(), path.string());
    }
}

}// namespace duk::shader_generator
