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

static std::string shader_data_source_class_name(const Parser& parser) {
    return duk::tools::snake_to_pascal(parser.output_material_name()) + "ShaderDataSource";
}

static std::string module_name(duk::rhi::ShaderModule::Bits moduleBit) {
    switch (moduleBit) {
        case duk::rhi::ShaderModule::VERTEX:
            return "duk::rhi::ShaderModule::VERTEX";
        case duk::rhi::ShaderModule::TESSELLATION_CONTROL:
            return "duk::rhi::ShaderModule::TESSELLATION_CONTROL";
        case duk::rhi::ShaderModule::TESSELLATION_EVALUATION:
            return "duk::rhi::ShaderModule::TESSELLATION_EVALUATION";
        case duk::rhi::ShaderModule::GEOMETRY:
            return "duk::rhi::ShaderModule::GEOMETRY";
        case duk::rhi::ShaderModule::FRAGMENT:
            return "duk::rhi::ShaderModule::FRAGMENT";
        case duk::rhi::ShaderModule::COMPUTE:
            return "duk::rhi::ShaderModule::COMPUTE";
    }
    throw std::runtime_error("Unknown shader module");
}

static std::string descriptor_type_name(duk::rhi::DescriptorType type) {
    switch (type) {
        case duk::rhi::DescriptorType::UNDEFINED:
            return "duk::rhi::DescriptorType::UNDEFINED";
        case duk::rhi::DescriptorType::UNIFORM_BUFFER:
            return "duk::rhi::DescriptorType::UNIFORM_BUFFER";
        case duk::rhi::DescriptorType::STORAGE_BUFFER:
            return "duk::rhi::DescriptorType::STORAGE_BUFFER";
        case duk::rhi::DescriptorType::IMAGE:
            return "duk::rhi::DescriptorType::IMAGE";
        case duk::rhi::DescriptorType::IMAGE_SAMPLER:
            return "duk::rhi::DescriptorType::IMAGE_SAMPLER";
        case duk::rhi::DescriptorType::STORAGE_IMAGE:
            return "duk::rhi::DescriptorType::STORAGE_IMAGE";
    }
    throw std::runtime_error("Unknown descriptor type");
}

static std::string vertex_attribute_format_name(duk::rhi::VertexInput::Format format) {
    switch (format) {
        case duk::rhi::VertexInput::Format::UNDEFINED:
            return "duk::rhi::VertexInput::Format::UNDEFINED";
        case duk::rhi::VertexInput::Format::INT8:
            return "duk::rhi::VertexInput::Format::INT8";
        case duk::rhi::VertexInput::Format::UINT8:
            return "duk::rhi::VertexInput::Format::UINT8";
        case duk::rhi::VertexInput::Format::INT16:
            return "duk::rhi::VertexInput::Format::INT16";
        case duk::rhi::VertexInput::Format::UINT16:
            return "duk::rhi::VertexInput::Format::UINT16";
        case duk::rhi::VertexInput::Format::INT32:
            return "duk::rhi::VertexInput::Format::INT32";
        case duk::rhi::VertexInput::Format::UINT32:
            return "duk::rhi::VertexInput::Format::UINT32";
        case duk::rhi::VertexInput::Format::FLOAT32:
            return "duk::rhi::VertexInput::Format::FLOAT32";
        case duk::rhi::VertexInput::Format::VEC2:
            return "duk::rhi::VertexInput::Format::VEC2";
        case duk::rhi::VertexInput::Format::VEC3:
            return "duk::rhi::VertexInput::Format::VEC3";
        case duk::rhi::VertexInput::Format::VEC4:
            return "duk::rhi::VertexInput::Format::VEC4";
    }
    throw std::runtime_error("unknown VertexInput::Format");
}

static std::string generate_module_mask(const Reflector& reflector) {
    std::ostringstream oss;
    const auto& modules = reflector.modules();
    bool needOperatorOr = false;
    for (const auto& [module, code]: modules) {
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

static std::string generate_spir_v_code_map(const Reflector& reflector) {
    std::ostringstream oss;
    oss << "{" << std::endl;
    const auto& modules = reflector.modules();
    bool needsComma = false;
    for (const auto& [module, code]: modules) {
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

static std::string generate_module_mask(duk::rhi::ShaderModule::Mask mask) {
    std::ostringstream oss;

    bool needsOrOperator = false;
    for (int i = 0; i < duk::rhi::ShaderModule::kCount; i++) {
        const auto bit = static_cast<duk::rhi::ShaderModule::Bits>(mask & (1 << i));
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
        for (const auto& set: sets) {
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
    } else {
        oss << '{' << std::endl;

        bool needsComma = false;
        for (auto format: reflector.attributes()) {
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