/// 04/11/2023
/// shader_data_source_generator.cpp

#include <duk_painter_generator/file_generators/shader_data_source_file_generator.h>
#include "duk_rhi/pipeline/shader_data_source.h"

#include <filesystem>
#include <regex>

namespace duk::painter_generator {

namespace detail {

static std::string shader_data_source_file_name(const Parser& parser) {
    return parser.output_painter_name() + "_shader_data_source";
}

static std::string shader_data_source_class_name(const Parser& parser) {
    auto painterName = parser.output_painter_name();
    painterName[0] = std::toupper(painterName[0]);
    return painterName + "ShaderDataSource";
}

static std::string module_name(duk::rhi::Shader::Module::Bits moduleBit) {
    switch (moduleBit) {
        case duk::rhi::Shader::Module::VERTEX: return "duk::rhi::Shader::Module::VERTEX";
        case duk::rhi::Shader::Module::TESSELLATION_CONTROL: return "duk::rhi::Shader::Module::TESSELLATION_CONTROL";
        case duk::rhi::Shader::Module::TESSELLATION_EVALUATION: return "duk::rhi::Shader::Module::TESSELLATION_EVALUATION";
        case duk::rhi::Shader::Module::GEOMETRY: return "duk::rhi::Shader::Module::GEOMETRY";
        case duk::rhi::Shader::Module::FRAGMENT: return "duk::rhi::Shader::Module::FRAGMENT";
        case duk::rhi::Shader::Module::COMPUTE: return "duk::rhi::Shader::Module::COMPUTE";
    }
    throw std::runtime_error("Unknown shader module");
}

static std::string descriptor_type_name(duk::rhi::DescriptorType type) {
    switch (type) {
        case duk::rhi::DescriptorType::UNDEFINED: return "duk::rhi::DescriptorType::UNDEFINED";
        case duk::rhi::DescriptorType::UNIFORM_BUFFER: return "duk::rhi::DescriptorType::UNIFORM_BUFFER";
        case duk::rhi::DescriptorType::STORAGE_BUFFER: return "duk::rhi::DescriptorType::STORAGE_BUFFER";
        case duk::rhi::DescriptorType::IMAGE: return "duk::rhi::DescriptorType::IMAGE";
        case duk::rhi::DescriptorType::IMAGE_SAMPLER: return "duk::rhi::DescriptorType::IMAGE_SAMPLER";
        case duk::rhi::DescriptorType::STORAGE_IMAGE: return "duk::rhi::DescriptorType::STORAGE_IMAGE";
    }
    throw std::runtime_error("Unknown descriptor type");
}

static std::string vertex_attribute_format_name(duk::rhi::VertexInput::Format format) {
    switch (format) {
        case duk::rhi::VertexInput::Format::UNDEFINED: return "duk::rhi::VertexInput::Format::UNDEFINED";
        case duk::rhi::VertexInput::Format::INT8: return "duk::rhi::VertexInput::Format::INT8";
        case duk::rhi::VertexInput::Format::UINT8: return "duk::rhi::VertexInput::Format::UINT8";
        case duk::rhi::VertexInput::Format::INT16: return "duk::rhi::VertexInput::Format::INT16";
        case duk::rhi::VertexInput::Format::UINT16: return "duk::rhi::VertexInput::Format::UINT16";
        case duk::rhi::VertexInput::Format::INT32: return "duk::rhi::VertexInput::Format::INT32";
        case duk::rhi::VertexInput::Format::UINT32: return "duk::rhi::VertexInput::Format::UINT32";
        case duk::rhi::VertexInput::Format::FLOAT32: return "duk::rhi::VertexInput::Format::FLOAT32";
        case duk::rhi::VertexInput::Format::VEC2: return "duk::rhi::VertexInput::Format::VEC2";
        case duk::rhi::VertexInput::Format::VEC3: return "duk::rhi::VertexInput::Format::VEC3";
        case duk::rhi::VertexInput::Format::VEC4: return "duk::rhi::VertexInput::Format::VEC4";
    }
    throw std::runtime_error("unknown VertexInput::Format");
}

static std::string generate_module_mask(const Reflector& reflector) {
    std::ostringstream oss;
    const auto& modules = reflector.modules();
    bool needOperatorOr = false;
    for (const auto& [module, code] : modules) {
        if (needOperatorOr) {
            oss << " | ";
        }
        needOperatorOr = true;
        oss << module_name(module);
    }
    return oss.str();
}

static std::string generate_spir_v_code(const std::vector<uint8_t>& code) {
    std::ostringstream oss;
    oss << '{' << std::endl << "        ";
    bool needsComma = false;
    int lineSize = 0;
    for (uint8_t byte : code) {
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

static std::string generate_spir_v_code_map(const Reflector& reflector) {
    std::ostringstream oss;
    oss << "{" << std::endl;
    const auto& modules = reflector.modules();
    bool needsComma = false;
    for (const auto& [module, code] : modules) {
        if (needsComma) {
            oss << ',' << std::endl;
        }
        needsComma = true;
        oss << "    {" << std::endl;
        oss << "        " << module_name(module) << ", " << generate_spir_v_code(code);
        oss << "    }";
    }
    oss << std::endl << '}';

    return oss.str();
}

static std::string generate_module_mask(duk::rhi::Shader::Module::Mask mask) {
    std::ostringstream oss;

    bool needsOrOperator = false;
    for (int i = 0; i < duk::rhi::Shader::Module::kCount; i++) {
        const auto bit = static_cast<duk::rhi::Shader::Module::Bits>(mask & (1 << i));
        if (bit) {
            if (needsOrOperator) {
                oss << " | ";
            }
            needsOrOperator = true;
            oss << module_name(bit);
        }
    }

    return oss.str();
}

static std::string generate_descriptor_sets_description(const Reflector& reflector) {
    std::ostringstream oss;
    oss << "{" << std::endl;
    const auto& sets = reflector.sets();

    if (!sets.empty()) {
        bool needsSetComma = false;
        for (const auto& set : sets) {
            if (needsSetComma) {
                oss << ',' << std::endl;
            }
            needsSetComma = true;
            oss << "    {" << std::endl;
            oss << "        {" << std::endl;

            bool needsBindingComma = false;
            for (const auto& binding: set.bindings) {
                if (needsBindingComma) {
                    oss << ',' << std::endl;
                }
                needsBindingComma = true;
                oss << "            {" << std::endl;
                oss << "                " << descriptor_type_name(binding.descriptorType) << ',' << std::endl;
                oss << "                " << generate_module_mask(binding.moduleMask) << std::endl;
                oss << "            }";
            }
            oss << std::endl << "        }";
            oss << std::endl << "    }";
        }
    }
    oss << std::endl << "}";

    return oss.str();
}

static std::string generate_vertex_layout(const Reflector& reflector) {
    std::ostringstream oss;

    if (reflector.attributes().empty()) {
        oss << "{}";
    }
    else {
        oss << '{' << std::endl;

        bool needsComma = false;
        for (auto format : reflector.attributes()) {
            if (needsComma) {
                oss << ',' << std::endl;
            }
            needsComma = true;
            oss << "    " << vertex_attribute_format_name(format);
        }
        oss << std::endl << "}";
    }

    return oss.str();
}

static const std::string kHeaderFileIncludes[] = {
    "duk_rhi/pipeline/shader_data_source.h"
};

/// header file template -----------------------------
/// --------------------------------------------------
const char* kClassDeclarationTemplate = R"(
class TemplateShaderDataSource : public duk::rhi::ShaderDataSource {
public:
    TemplateShaderDataSource();

    duk::rhi::Shader::Module::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::Shader::Module::Bits type) const override;

    const std::vector<duk::rhi::DescriptorSetDescription>& descriptor_set_descriptions() const override;

    const duk::rhi::VertexLayout& vertex_layout() const override;

private:
    duk::hash::Hash calculate_hash() const override;

private:
    duk::rhi::Shader::Module::Mask m_moduleMask;
    std::unordered_map<duk::rhi::Shader::Module::Bits, std::vector<uint8_t>> m_moduleSpirVCode;
    std::vector<duk::rhi::DescriptorSetDescription> m_descriptorSetDescriptions;
    duk::rhi::VertexLayout m_vertexLayout;
};)";
/// ---------------------------------------------------

/// source file template-------------------------------
const char* kClassDefinitionTemplate = R"(

TemplateShaderDataSource::TemplateShaderDataSource() {
    m_moduleMask = TemplateModuleMask;

    m_moduleSpirVCode = TemplateModuleSpirVCode;

    m_descriptorSetDescriptions = TemplateModuleDescriptorSetDescriptions;

    m_vertexLayout = TemplateVertexLayout;

    update_hash();
}

duk::rhi::Shader::Module::Mask TemplateShaderDataSource::module_mask() const {
    return m_moduleMask;
}

const std::vector<uint8_t>& TemplateShaderDataSource::shader_module_spir_v_code(duk::rhi::Shader::Module::Bits type) const {
    return m_moduleSpirVCode.at(type);
}

const std::vector<duk::rhi::DescriptorSetDescription>& TemplateShaderDataSource::descriptor_set_descriptions() const {
    return m_descriptorSetDescriptions;
}

const rhi::VertexLayout& TemplateShaderDataSource::vertex_layout() const {
    return m_vertexLayout;
}

duk::hash::Hash TemplateShaderDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;

    duk::hash::hash_combine(hash, m_moduleMask);
    for (const auto&[type, module] : m_moduleSpirVCode) {
        duk::hash::hash_combine(hash, type);
        duk::hash::hash_combine(hash, module.data(), module.size());
    }

    for (const auto& descriptorSetDescription : m_descriptorSetDescriptions) {
        duk::hash::hash_combine(hash, descriptorSetDescription.bindings.begin(), descriptorSetDescription.bindings.end());
    }

    duk::hash::hash_combine(hash, m_vertexLayout);

    return hash;
})";
/// ---------------------------------------------------

}

ShaderDataSourceFileGenerator::ShaderDataSourceFileGenerator(const Parser& parser, const Reflector& reflector) :
    m_parser(parser),
    m_reflector(reflector) {

    const auto& painterName = m_parser.output_painter_name();
    m_fileName = detail::shader_data_source_file_name(m_parser);
    m_headerIncludePath = "duk_renderer/materials/" + m_parser.output_painter_name() + '/' + m_fileName + ".h";
    m_className = detail::shader_data_source_class_name(m_parser);

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
    std::string includes[] = {
            m_headerIncludePath
    };
    generate_include_directives(oss, includes);
    oss << std::endl;
    generate_namespace_start(oss, "");

    generate_class_definition(oss);
    oss << std::endl;
    generate_namespace_end(oss, "");
}

void ShaderDataSourceFileGenerator::generate_class_declaration(std::ostringstream& oss) {
    auto classDeclaration = std::regex_replace(detail::kClassDeclarationTemplate, std::regex("TemplateShaderDataSource"), m_className);

    oss << classDeclaration << std::endl;
}

void ShaderDataSourceFileGenerator::generate_class_definition(std::ostringstream& oss) {
    auto classDefinition = std::regex_replace(detail::kClassDefinitionTemplate, std::regex("TemplateShaderDataSource"), m_className);
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateModuleMask"), detail::generate_module_mask(m_reflector));
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateModuleSpirVCode"), detail::generate_spir_v_code_map(m_reflector));
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateModuleDescriptorSetDescriptions"), detail::generate_descriptor_sets_description(m_reflector));
    classDefinition = std::regex_replace(classDefinition, std::regex("TemplateVertexLayout"), detail::generate_vertex_layout(m_reflector));

    oss << classDefinition << std::endl;
}

const std::string& ShaderDataSourceFileGenerator::output_header_include_path() const {
    return m_headerIncludePath;
}

const std::string& ShaderDataSourceFileGenerator::output_shader_data_source_class_name() const {
    return m_className;
}


}