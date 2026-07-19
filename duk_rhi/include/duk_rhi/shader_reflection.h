/// shader_reflection.h

#ifndef DUK_RHI_SHADER_REFLECTION_H
#define DUK_RHI_SHADER_REFLECTION_H

#include <duk_rhi/shader.h>
#include <duk_rhi/vertex_layout.h>

#include <duk_macros/macros.h>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct SpvReflectShaderModule;

namespace duk::rhi {

struct StructMember {
    std::string name;
    std::string typeName; // GLSL type name (e.g. "vec3", "DirectionalLight")
    uint32_t offset;
    uint32_t size;        // element size (may equal total size for non-arrays)
    uint32_t padding;     // bytes of padding after this member
};

struct StructDefinition {
    std::string name;
    uint32_t size; // padded element size of the struct (stride when used in an array)
    std::vector<StructMember> members;
};

using StructTypeMap = std::unordered_map<std::string, StructDefinition>;

class ShaderReflection {
public:
    /// Reflects all provided stages.
    /// Builds the merged binding layout and vertex layout immediately.
    explicit ShaderReflection(const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& stageSpirV);

    ~ShaderReflection();

    ShaderReflection(const ShaderReflection&) = delete;
    ShaderReflection& operator=(const ShaderReflection&) = delete;

    /// Reflects and returns the merged flat binding layout across all stages.
    /// A descriptor shared across stages appears once; its moduleMask accumulates all stages.
    DUK_NO_DISCARD ShaderBindingLayout binding_layout();

    /// Reflects and returns the vertex attribute layout from the VERTEX stage.
    /// Returns an empty layout if no vertex stage is present.
    DUK_NO_DISCARD VertexLayout vertex_layout();

    /// Patches the binding decorations in each stage's SPIR-V so that every descriptor
    /// lands on (set=0, binding=logicalIndex) as dictated by @p bindingLayout.
    DUK_NO_DISCARD std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> remap_bindings(const ShaderBindingLayout& bindingLayout);

    /// Recursively extracts all named struct type definitions referenced by buffer bindings
    /// across all reflected stages.
    DUK_NO_DISCARD StructTypeMap extract_struct_types();

private:
    struct SpvReflectShaderModuleDeleter {
        void operator()(SpvReflectShaderModule* module) const noexcept;
    };

    std::unordered_map<ShaderModule::Bits, std::unique_ptr<SpvReflectShaderModule, SpvReflectShaderModuleDeleter>> m_stageModules;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_REFLECTION_H

