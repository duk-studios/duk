/// 04/11/2023
/// local_types_file_generator.h

#ifndef DUK_MATERIAL_GENERATOR_TYPES_FILE_GENERATOR_H
#define DUK_MATERIAL_GENERATOR_TYPES_FILE_GENERATOR_H

#include <duk_material_generator/file_generators/file_generator.h>

#include <set>

namespace duk::material_generator {

class TypesFileGenerator : public FileGenerator {
public:
    TypesFileGenerator(const Parser& parser, const Reflector& reflector);

protected:
    void generate_includes(std::ostringstream& oss);

    void generate_type(std::ostringstream& oss, const TypeReflection& type, uint32_t indentationLevel = 0);

    void generate_types(std::ostringstream& oss, const std::vector<TypeReflection>& types, uint32_t indentationLevel = 0);

    std::string generate_types(const std::vector<TypeReflection>& types, uint32_t indentationLevel = 0);

    void generate_binding_alias(std::ostringstream& oss, const BindingReflection& bindingReflection);

    void generate_binding_aliases(std::ostringstream& oss, std::span<const BindingReflection> bindingReflections);

    std::vector<TypeReflection> extract_types(const BindingReflection& binding);

    std::vector<TypeReflection> extract_types(std::span<const BindingReflection> bindings);

    std::vector<TypeReflection> sort_types_by_declaration_order(const std::set<std::string>& types);

protected:
    const Parser& m_parser;
    const Reflector& m_reflector;
};

}// namespace duk::material_generator

#endif// DUK_MATERIAL_GENERATOR_TYPES_FILE_GENERATOR_H
