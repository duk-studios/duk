/// 04/11/2023
/// local_types_file_generator.cpp

#include <duk_painter_generator/file_generators/types_file_generator.h>
#include <duk_painter_generator/types.h>

#include <set>
#include <filesystem>

namespace duk::painter_generator {

namespace detail {

static std::string descriptor_type_name(duk::rhi::DescriptorType descriptorType) {
    switch (descriptorType) {
        case rhi::DescriptorType::UNIFORM_BUFFER:
            return "UniformBuffer";
        case rhi::DescriptorType::STORAGE_BUFFER:
            return "StorageBuffer";
        default:
            return "Unknown";
    }
}

struct TypeReferences {
    std::set<std::string> referencedTypes;
};

static uint32_t sum_reference_count(const std::unordered_map<std::string, TypeReferences>& typeReferences,
                                    const std::string& typeName) {

    auto it = typeReferences.find(typeName);
    if (it == typeReferences.end()) {
        return 0;
    }

    const auto& referencedTypes = it->second.referencedTypes;

    uint32_t sum = 0;
    for (const auto& referencedType : referencedTypes) {
        sum++;
        sum += sum_reference_count(typeReferences, referencedType);
    }

    return sum;
}

static void insert_referenced_types(std::set<std::string>& referencedTypeNames, const Reflector& reflector, const TypeReflection& type, bool insertType) {
    auto it = referencedTypeNames.find(type.name);
    if (it != referencedTypeNames.end()) {
        // already referenced
        return;
    }

    if (is_builtin_glsl_type(type.name)) {
        // no need to insert builtin types
        return;
    }

    // we do not include SBOs and UBOs into referenced types, only their members
    if (insertType) {
        referencedTypeNames.insert(type.name);
    }

    const auto& types = reflector.types();

    for (const auto& member : type.members) {
        insert_referenced_types(referencedTypeNames, reflector, types.at(member.typeName), true);
    }
}

static void insert_referenced_types_from_binding(const Reflector& reflector, std::set<std::string>& referencedTypes, const BindingReflection& binding) {
    const auto& types = reflector.types();
    if (binding.descriptorType != duk::rhi::DescriptorType::UNIFORM_BUFFER && binding.descriptorType != duk::rhi::DescriptorType::STORAGE_BUFFER) {
        return;
    }
    auto it = types.find(binding.typeName);
    detail::insert_referenced_types(referencedTypes, reflector, it->second, false);
}

static void insert_referenced_types_from_bindings(const Reflector& reflector, std::set<std::string>& referencedTypes, std::span<const BindingReflection> bindings) {
    for (const auto& binding : bindings) {
        insert_referenced_types_from_binding(reflector, referencedTypes, binding);
    }
}

static const std::string kTypesFileIncludes[] = {
        "duk_renderer/painters/uniform_buffer.h",
        "duk_renderer/painters/storage_buffer.h",
        "glm/glm.hpp"
};

}

TypesFileGenerator::TypesFileGenerator(const Parser& parser, const Reflector& reflector) :
    m_parser(parser),
    m_reflector(reflector){

}

void TypesFileGenerator::generate_includes(std::ostringstream& oss) {
    generate_include_directives(oss, detail::kTypesFileIncludes);
    oss << std::endl;
}

void TypesFileGenerator::generate_type(std::ostringstream& oss, const TypeReflection& type) {
    oss << "struct " << type.name << " {" << std::endl;
    for (auto& member : type.members) {
        oss << "    " << glsl_to_cpp(member.typeName) << " " << member.name;
        if (member.arraySize) {
            oss << '[' << member.arraySize << ']';
        }
        oss << ';' << std::endl;
        if (member.paddedSize > member.size) {
            auto padding = member.paddedSize - member.size;
            oss << "    uint8_t _padding_" << member.name << '[' << padding << "];" << std::endl;
        }
    }
    oss << "};" << std::endl;
}

void TypesFileGenerator::generate_types(std::ostringstream& oss, const std::vector<TypeReflection>& types) {
    for (const auto& type : types) {
        generate_type(oss, type);
        oss << std::endl;
    }
}

void TypesFileGenerator::generate_binding_alias(std::ostringstream& oss, const BindingReflection& binding) {
    if (binding.descriptorType != duk::rhi::DescriptorType::UNIFORM_BUFFER && binding.descriptorType != duk::rhi::DescriptorType::STORAGE_BUFFER) {
        return;
    }
    const auto& types = m_reflector.types();
    const auto& uboType = types.at(binding.typeName);
    const auto& memberTypeName = uboType.members.at(0).typeName;
    oss << "using " << binding.typeName << " = " << detail::descriptor_type_name(binding.descriptorType) << '<' << glsl_to_cpp(memberTypeName) << ">;" << std::endl;
}

void TypesFileGenerator::generate_binding_aliases(std::ostringstream& oss, std::span<const BindingReflection> bindingReflections) {
    for (const auto& bindingReflection : bindingReflections) {
        generate_binding_alias(oss, bindingReflection);
    }
}

std::vector<TypeReflection> TypesFileGenerator::extract_types(const BindingReflection& binding) {
    std::set<std::string> referencedTypeNames;

    detail::insert_referenced_types_from_binding(m_reflector, referencedTypeNames, binding);

    return sort_types_by_declaration_order(referencedTypeNames);
}

std::vector<TypeReflection> TypesFileGenerator::extract_types(std::span<const BindingReflection> bindings) {
    std::set<std::string> referencedTypeNames;

    detail::insert_referenced_types_from_bindings(m_reflector, referencedTypeNames, bindings);

    return sort_types_by_declaration_order(referencedTypeNames);
}

std::vector<TypeReflection> TypesFileGenerator::sort_types_by_declaration_order(const std::set<std::string>& typeNames) {

    const auto& types = m_reflector.types();
    // resolve references
    std::unordered_map<std::string, detail::TypeReferences> typeReferences;

    for (auto& typeName : typeNames) {
        auto& type = types.at(typeName);

        for (auto& member : type.members) {
            if (typeNames.find(member.typeName) != typeNames.end()) {
                typeReferences[typeName].referencedTypes.insert(member.typeName);
            }
        }
    }

    // sort based on reference tree
    std::vector<std::string> sortedTypeNames(typeNames.begin(), typeNames.end());

    std::sort(sortedTypeNames.begin(), sortedTypeNames.end(), [&typeReferences](const std::string& lhs, const std::string& rhs) {
        return sum_reference_count(typeReferences, lhs) < sum_reference_count(typeReferences, rhs);
    });

    std::vector<TypeReflection> sortedTypes(sortedTypeNames.size());

    for (auto i = 0; i < sortedTypes.size(); i++) {
        sortedTypes[i] = types.at(sortedTypeNames[i]);
    }

    return sortedTypes;
}

}