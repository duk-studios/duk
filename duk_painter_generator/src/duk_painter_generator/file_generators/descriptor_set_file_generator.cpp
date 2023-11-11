/// 11/11/2023
/// descriptor_set_generator.cpp

#include <duk_painter_generator/file_generators/descriptor_set_file_generator.h>
#include <duk_painter_generator/file_generators/generator_utils.h>

#include <filesystem>
#include <regex>

namespace duk::painter_generator {

namespace detail {

std::string descriptor_set_file_name(const Parser& parser) {
    return parser.output_painter_name() + "_descriptor_sets";
}

std::string descriptor_set_class_name(const Parser& parser) {
    auto painterName = parser.output_painter_name();
    painterName[0] = std::toupper(painterName[0]);
    return painterName + "DescriptorSet";
}

std::string generate_bindings(const Reflector::Bindings& bindings) {
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

const char* kDescriptorSetClassDeclarationTemplate = R"(
struct TemplateClassNameCreateInfo {
    duk::rhi::RHI* rhi;
    const TemplateShaderDataSourceClassName* shaderDataSource;
};

class TemplateClassName {
public:

    enum class Bindings : uint32_t TemplateBindings;

public:

    TemplateClassName(const TemplateClassNameCreateInfo& descriptorSetCreateInfo);

    void set(Bindings binding, const duk::rhi::Descriptor& descriptor);

    duk::rhi::Descriptor& at(Bindings binding);

    duk::rhi::Descriptor& at(uint32_t bindingIndex);

    duk::rhi::DescriptorSet* handle();

    void flush();

private:
    std::shared_ptr<duk::rhi::DescriptorSet> m_descriptorSet;
};
)";

const char* kDescriptorSetClassImplementationTemplate = R"(
TemplateClassName::TemplateClassName(const TemplateClassNameCreateInfo& createInfo) {

    duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.description = createInfo.shaderDataSource->descriptor_set_descriptions().at(0);

    auto result = createInfo.rhi->create_descriptor_set(descriptorSetCreateInfo);

    if (!result) {
        throw std::runtime_error("failed to create TemplateClassName: " + result.error().description());
    }

    m_descriptorSet = std::move(result.value());
}

void TemplateClassName::set(TemplateClassName::Bindings binding, const duk::rhi::Descriptor& descriptor) {
    m_descriptorSet->set(static_cast<uint32_t>(binding), descriptor);
}

duk::rhi::Descriptor& TemplateClassName::at(TemplateClassName::Bindings binding) {
    return m_descriptorSet->at(static_cast<uint32_t>(binding));
}

duk::rhi::Descriptor& TemplateClassName::at(uint32_t bindingIndex) {
    return m_descriptorSet->at(bindingIndex);
}

duk::rhi::DescriptorSet* TemplateClassName::handle() {
    return m_descriptorSet.get();
}

void TemplateClassName::flush() {
    m_descriptorSet->flush();
}
)";

}

DescriptorSetFileGenerator::DescriptorSetFileGenerator(const Parser& parser, const Reflector& reflector, const ShaderDataSourceFileGenerator& shaderDataSourceFileGenerator) :
    m_parser(parser),
    m_reflector(reflector),
    m_shaderDataSourceFileGenerator(shaderDataSourceFileGenerator) {

    m_fileName = detail::descriptor_set_file_name(m_parser);
    m_className = detail::descriptor_set_class_name(m_parser);
    const auto& painterName = m_parser.output_painter_name();

    {
        std::ostringstream oss;
        generate_header_file_content(oss);

        const auto filePath = std::filesystem::path(m_parser.output_include_directory()) / painterName / (m_fileName + ".h");

        write_file(oss.str(), filePath.string());
    }

    {
        std::ostringstream oss;
        generate_source_file_content(oss);

        const auto filePath = std::filesystem::path(m_parser.output_source_directory()) / painterName / (m_fileName + ".cpp");

        write_file(oss.str(), filePath.string());
    }

}

void DescriptorSetFileGenerator::generate_header_file_content(std::ostringstream& oss) {
    std::string includes[] = {
            m_shaderDataSourceFileGenerator.output_header_include_path(),
            "duk_rhi/rhi.h"
    };

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
            "duk_renderer/painters/" + m_parser.output_painter_name() + '/' + m_fileName + ".h"
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

    oss << classDeclaration;
}

void DescriptorSetFileGenerator::generate_class_definition(std::ostringstream& oss) {
    auto classDefinition = std::regex_replace(detail::kDescriptorSetClassImplementationTemplate, std::regex("TemplateClassName"), m_className);

    oss << classDefinition;
}

}