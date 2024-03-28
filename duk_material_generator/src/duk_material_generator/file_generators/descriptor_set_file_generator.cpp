/// 11/11/2023
/// descriptor_set_generator.cpp

#include <duk_material_generator/file_generators/descriptor_set_file_generator.h>
#include <duk_material_generator/file_generators/generator_utils.h>
#include <duk_tools/string.h>

#include <filesystem>
#include <regex>

namespace duk::material_generator {

namespace detail {

static std::string descriptor_set_file_name(const Parser& parser) {
    return parser.output_material_name() + "_descriptors";
}

static std::string descriptor_set_class_name(const Parser& parser) {
    return duk::tools::snake_to_pascal(parser.output_material_name()) + "Descriptors";
}

static std::string generate_bindings(const Reflector::Bindings& bindings) {
    std::ostringstream oss;
    oss << '{' << std::endl;
    oss << utils::generate_for_each(bindings, [](const BindingReflection& binding) -> std::string {
        std::ostringstream oss;
        oss << "        " << binding.name << " = " << binding.binding;
        return oss.str();
    });
    oss << std::endl << "    }";
    return oss.str();
}

static std::vector<BindingReflection> extract_local_bindings(const Parser& parser, std::span<const BindingReflection> bindings) {
    std::vector<BindingReflection> localBindings;
    for (auto& binding: bindings) {
        if (parser.is_global_binding(binding.typeName)) {
            continue;
        }
        localBindings.push_back(binding);
    }
    return localBindings;
}

static std::string descriptor_set_header_include_path(const Parser& parser, const std::string& fileName) {
    auto absoluteIncludeDirectory = parser.output_include_directory();
    auto startIncludePos = absoluteIncludeDirectory.find("duk_renderer/material/");
    auto relativeIncludeDirectory = absoluteIncludeDirectory.substr(startIncludePos);
    std::ostringstream oss;
    oss << relativeIncludeDirectory << '/' << fileName << ".h";
    return oss.str();
}

const char* kDescriptorSetClassDeclarationTemplate = R"(
class TemplateClassName {
public:

    enum Bindings : uint32_t TemplateBindings;

    static constexpr uint32_t kDescriptorCount = TemplateBindingCount;

TemplateTypes
};
)";

const char* kDescriptorSetClassImplementationTemplate = R"(


)";

}// namespace detail

DescriptorSetFileGenerator::DescriptorSetFileGenerator(const Parser& parser, const Reflector& reflector, const ShaderDataSourceFileGenerator& shaderDataSourceFileGenerator)
    : TypesFileGenerator(parser, reflector)
    , m_shaderDataSourceFileGenerator(shaderDataSourceFileGenerator) {
    m_fileName = detail::descriptor_set_file_name(m_parser);
    m_className = detail::descriptor_set_class_name(m_parser);

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

void DescriptorSetFileGenerator::generate_header_file_content(std::ostringstream& oss) {
    std::string includes[] = {"duk_renderer/material/material_descriptor_set.h", m_shaderDataSourceFileGenerator.output_header_include_path(), "duk_rhi/rhi.h"};

    generate_include_guard_start(oss, m_fileName);
    oss << std::endl;
    generate_include_directives(oss, includes);
    oss << std::endl;
    generate_namespace_start(oss, "");

    generate_class_declaration(oss, m_reflector.sets().at(0));
    oss << std::endl;

    generate_namespace_end(oss, "");
    oss << std::endl;
    generate_include_guard_end(oss, m_fileName);
}

void DescriptorSetFileGenerator::generate_source_file_content(std::ostringstream& oss) {
    std::string includes[] = {
            detail::descriptor_set_header_include_path(m_parser, m_fileName),
    };
    generate_include_directives(oss, includes);
    oss << std::endl;
    generate_namespace_start(oss, "");

    generate_class_definition(oss);
    oss << std::endl;
    generate_namespace_end(oss, "");
}

void DescriptorSetFileGenerator::generate_class_declaration(std::ostringstream& oss, const SetReflection& descriptorSet) {
    auto classDeclaration = std::regex_replace(detail::kDescriptorSetClassDeclarationTemplate, std::regex("TemplateClassName"), m_className);
    classDeclaration = std::regex_replace(classDeclaration, std::regex("TemplateShaderDataSourceClassName"), m_shaderDataSourceFileGenerator.output_shader_data_source_class_name());
    classDeclaration = std::regex_replace(classDeclaration, std::regex("TemplateBindings"), detail::generate_bindings(descriptorSet.bindings));
    classDeclaration = std::regex_replace(classDeclaration, std::regex("TemplateBindingCount"), std::to_string(descriptorSet.bindings.size()));
    classDeclaration = std::regex_replace(classDeclaration, std::regex("TemplateTypes"), generate_types(extract_types(detail::extract_local_bindings(m_parser, descriptorSet.bindings)), 4));
    oss << classDeclaration;
}

void DescriptorSetFileGenerator::generate_class_definition(std::ostringstream& oss) {
    auto classDefinition = std::regex_replace(detail::kDescriptorSetClassImplementationTemplate, std::regex("TemplateClassName"), m_className);

    oss << classDefinition;
}

}// namespace duk::material_generator