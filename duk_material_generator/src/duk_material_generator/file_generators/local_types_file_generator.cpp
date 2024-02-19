/// 04/11/2023
/// local_types_file_generator.cpp

#include <duk_material_generator/file_generators/local_types_file_generator.h>
#include <duk_material_generator/types.h>

#include <filesystem>

namespace duk::material_generator {


LocalTypesFileGenerator::LocalTypesFileGenerator(const Parser& parser, const Reflector& reflector) :
    TypesFileGenerator(parser, reflector) {

    std::ostringstream oss;
    generate_file_content(oss);

    const auto& materialName = parser.output_material_name();

    auto filepath = std::filesystem::path(parser.output_include_directory()) / (materialName + "_types.h");

    write_file(oss.str(), filepath.string());

}

void LocalTypesFileGenerator::generate_file_content(std::ostringstream& oss) {
    const auto fileName = m_parser.output_material_name() + "_types";
    generate_include_guard_start(oss, fileName);
    oss << std::endl;
    generate_includes(oss);

    generate_namespace_start(oss, m_parser.output_material_name());
    oss << std::endl;

    const auto bindings = extract_local_bindings();

    generate_types(oss, extract_types(bindings));

    generate_binding_aliases(oss, bindings);

    oss << std::endl;
    generate_namespace_end(oss, m_parser.output_material_name());
    oss << std::endl;
    generate_include_guard_end(oss, fileName);
}

std::vector<BindingReflection> LocalTypesFileGenerator::extract_local_bindings() {
    const auto& sets = m_reflector.sets();
    std::vector<BindingReflection> bindings;
    for (const auto& set : sets) {
        for (auto& binding : set.bindings) {
            if (m_parser.is_global_binding(binding.typeName)) {
                continue;
            }
            bindings.push_back(binding);
        }
    }
    return bindings;
}
}

