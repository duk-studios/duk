/// 15/10/2023
/// types.cpp

#include <duk_painter_generator/types.h>

#include <unordered_map>
#include <unordered_set>

namespace duk::painter_generator {

const std::string& glsl_to_cpp(const std::string& glslTypeName) {
    static const auto glslToCppMap = []() -> std::unordered_map<std::string, std::string> {
        return {
                {"vec2", "glm::vec2"},
                {"vec3", "glm::vec3"},
                {"vec4", "glm::vec4"},

                {"dvec2", "glm::dvec2"},
                {"dvec3", "glm::dvec3"},
                {"dvec4", "glm::dvec4"},

                {"ivec2", "glm::ivec2"},
                {"ivec3", "glm::ivec3"},
                {"ivec4", "glm::ivec4"},

                {"uvec2", "glm::uvec2"},
                {"uvec3", "glm::uvec3"},
                {"uvec4", "glm::uvec4"},

                {"mat2x4", "glm::mat2x4"},
                {"mat3x4", "glm::mat3x4"},
                {"mat4x4", "glm::mat4x4"},

                {"mat2x3", "glm::mat2x3"},
                {"mat3x3", "glm::mat3x3"},
                {"mat4x3", "glm::mat4x3"},

                {"mat2x2", "glm::mat2x2"},
                {"mat3x2", "glm::mat3x2"},
                {"mat4x2", "glm::mat4x2"},
        };
    }();

    auto it = glslToCppMap.find(glslTypeName);
    if (it == glslToCppMap.end()) {
        return glslTypeName;
    }
    return it->second;
}

bool is_global_binding(const std::string& typeName) {
    static const auto globalBindingSet = []() -> std::unordered_set<std::string> {
        return {
            "LightsUBO",
            "CameraUBO"
        };
    }();
    return globalBindingSet.find(typeName) != globalBindingSet.end();
}

bool is_builtin_glsl_type(const std::string& glslTypeName) {
    static auto builtinGlslTypeNameSet = []() -> std::unordered_set<std::string> {
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
    return builtinGlslTypeNameSet.find(glslTypeName) != builtinGlslTypeNameSet.end();
}

}