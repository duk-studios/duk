/// 04/11/2023
/// shader_data_source_generator.cpp

#include <duk_shader_generator/file_generators/shader_data_source_file_generator.h>
#include <duk_tools/string.h>

#include <filesystem>
#include <regex>

namespace duk::material_generator {

namespace detail {

static std::string shader_data_source_file_name(const Parser& parser) {
    return parser.output_material_name() + "_shader_data_source";
}

static std::string shader_header_include_path(const Parser& parser, const std::string& fileName) {
    auto absoluteIncludeDirectory = parser.output_include_directory();
    auto startIncludePos = absoluteIncludeDirectory.find("duk_renderer/shader/");
    auto relativeIncludeDirectory = absoluteIncludeDirectory.substr(startIncludePos);
    std::ostringstream oss;
    oss << relativeIncludeDirectory << '/' << fileName << ".h";
    return oss.str();
}

static std::string generate_spir_v_code(const std::vector<uint8_t>& code) {
    std::ostringstream oss;
    oss << '{' << std::endl << "        ";
    bool needsComma = false;
    int lineSize = 0;
    for (uint8_t byte: code) {
        if (needsComma) {
            oss << ',';
        }
        needsComma = true;

        if (lineSize > 24) {
            oss << std::endl << "        ";
            lineSize = 0;
        }

        oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        lineSize++;
    }
    oss << std::endl << "        }" << std::endl;
    return oss.str();
}

static const std::string kHeaderFileIncludes[] = {"vector", "cstdint"};

/// header file template -----------------------------
/// --------------------------------------------------
const char* kClassDeclarationTemplate = R"(

extern std::vector<uint8_t> TemplateShaderName_vert_spir_v();

extern std::vector<uint8_t> TemplateShaderName_frag_spir_v();

)";
/// ---------------------------------------------------

/// source file template-------------------------------
const char* kClassDefinitionTemplate = R"(

std::vector<uint8_t> TemplateShaderName_vert_spir_v() {
    return TemplateModuleVertSpirvCode;
}

std::vector<uint8_t> TemplateShaderName_frag_spir_v() {
    return TemplateModuleFragSpirvCode;
}

)";
/// ---------------------------------------------------

}// namespace detail

ShaderDataSourceFileGenerator::ShaderDataSourceFileGenerator(const Parser& parser, const Reflector& reflector)
    : m_parser(parser)
    , m_reflector(reflector) {
    m_fileName = detail::shader_data_source_file_name(m_parser);
    m_headerIncludePath = detail::shader_header_include_path(m_parser, m_fileName);
    m_className = m_parser.output_material_name();

    {
        std::ostringstream oss;
        generate_header_file_content(oss);

        const auto filePath = std::filesystem::path(m_parser.output_include_directory()) / (m_fileName + ".h");

        write_file(oss.str(), filePath.string());
    }

    {
        std::ostringstream oss;
        generate_source_file_content(oss);

        const auto filePath = std::filesystem::path(m_parser.output_source_directory()) / (m_fileName + ".cpp");

        write_file(oss.str(), filePath.string());
    }
}

void ShaderDataSourceFileGenerator::generate_header_file_content(std::ostringstream& oss) {
    generate_include_guard_start(oss, m_fileName);
    oss << std::endl;
    generate_include_directives(oss, detail::kHeaderFileIncludes);
    oss << std::endl;
    generate_namespace_start(oss, "");

    generate_class_declaration(oss);
    oss << std::endl;

    generate_namespace_end(oss, "");
    oss << std::endl;
    generate_include_guard_end(oss, m_fileName);
}

void ShaderDataSourceFileGenerator::generate_source_file_content(std::ostringstream& oss) {
    std::string includes[] = {m_headerIncludePath};
    generate_include_directives(oss, includes);
    oss << std::endl;
    generate_namespace_start(oss, "");

    generate_class_definition(oss);
    oss << std::endl;
    generate_namespace_end(oss, "");
}

void ShaderDataSourceFileGenerator::generate_class_declaration(std::ostringstream& oss) {
    auto classDeclaration = std::regex_replace(detail::kClassDeclarationTemplate, std::regex("TemplateShaderName"), m_className);

    oss << classDeclaration << std::endl;
}

void ShaderDataSourceFileGenerator::generate_class_definition(std::ostringstream& oss) {
    auto classDefinition = std::regex_replace(detail::kClassDefinitionTemplate, std::regex("TemplateShaderName"), m_className);
    const auto& modules = m_reflector.modules();
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateModuleVertSpirvCode"), detail::generate_spir_v_code(modules.at(duk::rhi::ShaderModule::VERTEX)));
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateModuleFragSpirvCode"), detail::generate_spir_v_code(modules.at(duk::rhi::ShaderModule::FRAGMENT)));

    oss << classDefinition << std::endl;
}

const std::string& ShaderDataSourceFileGenerator::output_header_include_path() const {
    return m_headerIncludePath;
}

const std::string& ShaderDataSourceFileGenerator::output_shader_data_source_class_name() const {
    return m_className;
}

}// namespace duk::material_generator