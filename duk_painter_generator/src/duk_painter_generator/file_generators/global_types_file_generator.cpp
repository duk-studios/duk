/// 04/11/2023
/// global_types_file_generator.cpp

#include <duk_painter_generator/file_generators/global_types_file_generator.h>
#include <duk_painter_generator/types.h>

#include <filesystem>
#include <regex>

namespace duk::painter_generator {

namespace detail {

static constexpr std::string kNamespaceName = "globals";

static std::string binding_name_to_file_name(const std::string& bindingName) {
    // Remove "UBO" or "SSBO" if present in the input string.
    std::string withoutUBOorSSBO = std::regex_replace(bindingName, std::regex("(UBO|SSBO)"), "");

    // Use a regular expression to insert '_' before an uppercase letter
    // if it is not followed by another uppercase letter.
    std::string fileName = std::regex_replace(withoutUBOorSSBO, std::regex("([a-z])([A-Z])"), "$1_$2");

    // Convert the result to lowercase.
    for (char& c : fileName) {
        c = std::tolower(c);
    }

    return fileName;
}

}

GlobalTypesFileGenerator::GlobalTypesFileGenerator(const Parser& parser, const Reflector& reflector) :
    TypesFileGenerator(parser, reflector) {

    const auto globalBindings = extract_global_bindings();

    for (auto& binding : globalBindings) {

        std::ostringstream oss;
        generate_file_content(oss, binding);

        const auto filepath = std::filesystem::path(parser.output_include_directory()) / detail::kNamespaceName / (detail::binding_name_to_file_name(binding.typeName) + "_types.h");

        std::filesystem::create_directories(filepath.parent_path());

        write_file(oss.str(), filepath.string());
    }

}

void GlobalTypesFileGenerator::generate_file_content(std::ostringstream& oss, const BindingReflection& binding) {

    generate_file_header(oss);

    generate_namespace_start(oss, detail::kNamespaceName);
    oss << std::endl;

    generate_types(oss, extract_types(binding));

    generate_binding_alias(oss, binding);

    oss << std::endl;
    generate_namespace_end(oss, detail::kNamespaceName);
}

std::vector<BindingReflection> GlobalTypesFileGenerator::extract_global_bindings() {
    std::vector<BindingReflection> globalBindings;
    const auto& sets = m_reflector.sets();
    for (const auto& set : sets) {
        for (const auto& binding : set.bindings) {
            if (m_parser.is_global_binding(binding.typeName)) {
                globalBindings.push_back(binding);
            }
        }
    }
    return globalBindings;
}

}