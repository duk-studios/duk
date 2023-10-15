/// 14/10/2023
/// generator.cpp

#include <duk_painter_generator/generator.h>
#include <duk_painter_generator/types.h>

#include <sstream>
#include <set>
#include <filesystem>
#include <fstream>
#include <span>

namespace duk::painter_generator {

namespace detail {

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

    const auto& types = reflector.type_map();

    for (const auto& member : type.members) {
        insert_referenced_types(referencedTypeNames, reflector, types.at(member.typeName), true);
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

static std::vector<TypeReflection> sort_types_by_declaration_order(const Reflector& reflector, const std::set<std::string>& typeNames) {

    const auto& types = reflector.type_map();

    // resolve references
    std::unordered_map<std::string, TypeReferences> typeReferences;

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

static void insert_referenced_types_from_bindings(std::set<std::string>& referencedTypes, const Reflector& reflector, const Reflector::Bindings& bindings, bool insertBindingType) {
    const auto& types = reflector.type_map();
    for (auto& [index, binding] : bindings) {
        if (is_global_binding(binding.typeName)) {
            continue;
        }
        auto it = types.find(binding.typeName);
        insert_referenced_types(referencedTypes, reflector, it->second, insertBindingType);
    }
}

static std::vector<TypeReflection> extract_painter_types(const Reflector& reflector) {
    const auto& sets = reflector.sets();

    std::set<std::string> referencedTypeNames;

    for (auto& set : sets) {
        insert_referenced_types_from_bindings(referencedTypeNames, reflector, set.uniformBuffers, false);
        insert_referenced_types_from_bindings(referencedTypeNames, reflector, set.storageBuffers, false);
    }

    return sort_types_by_declaration_order(reflector, referencedTypeNames);
}

static void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include : includes) {
        oss << "#include <" << include << '>' << std::endl;
    }
}

static void generate_namespace_start(std::ostringstream& oss, const std::string& painterName) {
    oss << "namespace duk::renderer::" << painterName << " {" << std::endl;
}

static void generate_namespace_end(std::ostringstream& oss, const std::string& painterName) {
    oss << "} // namespace duk::renderer::" << painterName << std::endl;
}

static void generate_type(std::ostringstream& oss, const TypeReflection& type) {
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

static void generate_painter_types(std::ostringstream& oss, const std::vector<TypeReflection>& types) {
    for (const auto& type : types) {
        generate_type(oss, type);
        oss << std::endl;
    }
}

static void generate_ubo_alias(std::ostringstream& oss, const Reflector& reflector, const BindingReflection& bindingReflection) {
    const auto& types = reflector.type_map();
    const auto& uboType = types.at(bindingReflection.typeName);
    const auto& memberTypeName = uboType.members.at(0).typeName;
    oss << "using " << bindingReflection.typeName << " = UniformBuffer<" << memberTypeName << ">;" << std::endl;
}

static void generate_sbo_alias(std::ostringstream& oss, const Reflector& reflector, const BindingReflection& bindingReflection) {
    const auto& types = reflector.type_map();
    const auto& sboType = types.at(bindingReflection.typeName);
    const auto& memberTypeName = sboType.members.at(0).typeName;
    oss << "using " << bindingReflection.typeName << " = StorageBuffer<" << memberTypeName << ">;" << std::endl;
}

static void generate_binding_alias(std::ostringstream& oss, const Reflector& reflector) {
    const auto& sets = reflector.sets();

    for (const auto& set : sets) {
        for (const auto& [index, ubo] : set.uniformBuffers) {
            if (is_global_binding(ubo.typeName)) {
                continue;
            }
            generate_ubo_alias(oss, reflector, ubo);
        }
        for (const auto& [index, ubo] : set.storageBuffers) {
            if (is_global_binding(ubo.typeName)) {
                continue;
            }
            generate_sbo_alias(oss, reflector, ubo);
        }
    }
}

static const std::string kBindingsHeaderIncludes[] = {
        "duk_renderer/painters/uniform_buffer.h",
        "duk_renderer/painters/storage_buffer.h",
        "glm/glm.hpp"
};

static void generate_painter_types_file(std::ostringstream& oss, const Parser& parser, const Reflector& reflector) {

    generate_include_directives(oss, kBindingsHeaderIncludes);
    oss << std::endl;
    generate_namespace_start(oss, parser.output_painter_name());
    oss << std::endl;
    generate_painter_types(oss, extract_painter_types(reflector));
    generate_binding_alias(oss, reflector);
    oss << std::endl;
    generate_namespace_end(oss, parser.output_painter_name());
}

static void write_file(const std::ostringstream& oss, const std::string& directory, const std::string& filename) {
    auto filepath = std::filesystem::path(directory) / filename;

    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath.string());
    }

    file << oss.str();
}

}

Generator::Generator(const Parser& parser, const Reflector& reflector) {
    std::ostringstream oss;
    detail::generate_painter_types_file(oss, parser, reflector);
    detail::write_file(oss, parser.output_directory(), parser.output_painter_name() + "_types.h");
}

Generator::~Generator() {

}

}