/// 14/10/2023
/// generator.cpp

#include <duk_painter_generator/generator.h>

#include <sstream>
#include <set>
#include <filesystem>
#include <fstream>

namespace duk::painter_generator {

namespace detail {

static bool is_builtin_type(const std::string& typeName) {
    static auto builtinTypeMap = []() -> std::set<std::string> {
        return {
                "int",
                "uint",
                "float",
                "double",
                "vec2",
                "vec3",
                "vec4",
                "dvec2",
                "dvec3",
                "dvec4",
                "uvec2",
                "uvec3",
                "uvec4",
                "ivec2",
                "ivec3",
                "ivec4",
                "mat2x2",
                "mat2x3",
                "mat2x4",
                "mat3x2",
                "mat3x3",
                "mat3x4",
                "mat4x2",
                "mat4x3",
                "mat4x4",
                "dmat2x2",
                "dmat2x3",
                "dmat2x4",
                "dmat3x2",
                "dmat3x3",
                "dmat3x4",
                "dmat4x2",
                "dmat4x3",
                "dmat4x4"
        };
    }();
    return builtinTypeMap.find(typeName) != builtinTypeMap.end();
}

static void insert_referenced_types(std::set<std::string>& referencedTypeNames, const Reflector& reflector, const TypeReflection& type) {
    auto it = referencedTypeNames.find(type.name);
    if (it != referencedTypeNames.end()) {
        // already referenced
        return;
    }

    if (is_builtin_type(type.name)) {
        // no need to insert builtin types
        return;
    }

    referencedTypeNames.insert(type.name);

    const auto& types = reflector.type_map();

    for (const auto& member : type.members) {
        insert_referenced_types(referencedTypeNames, reflector, types.at(member.typeName));
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

static std::vector<TypeReflection> extract_types_for_generation(const Reflector& reflector) {
    const auto& sets = reflector.sets();
    const auto& types = reflector.type_map();

    std::set<std::string> referencedTypeNames;

    for (auto& set : sets) {
        for (auto& [index, binding] : set.bindings) {
            auto it = types.find(binding.typeName);
            insert_referenced_types(referencedTypeNames, reflector, it->second);
        }
    }

    return sort_types_by_declaration_order(reflector, referencedTypeNames);
}

static void generate_type(std::ostringstream& oss, const TypeReflection& type) {
    oss << "struct " << type.name << " {" << std::endl;
    for (auto& member : type.members) {
        oss << "    " << member.typeName << " " << member.name;
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

static void generate_types(std::ostringstream& oss, const std::vector<TypeReflection>& types) {
    for (const auto& type : types) {
        generate_type(oss, type);
        oss << std::endl;
    }
}

}

Generator::Generator(const Parser& parser, const Reflector& reflector) {
    auto typesForGeneration = detail::extract_types_for_generation(reflector);

    std::ostringstream oss;
    detail::generate_types(oss, typesForGeneration);

    auto outputDir = std::filesystem::path(parser.output_directory());
    auto bindingsFile = outputDir / (parser.output_painter_name() + "_bindings.h");

    std::ofstream file(bindingsFile);

    if (!file) {
        throw std::runtime_error("failed to open file at: " + bindingsFile.string());
    }

    file << oss.str();
}

Generator::~Generator() {

}

}