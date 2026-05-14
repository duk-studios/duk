//
// Created by Ricardo on 05/05/2024.
//

#ifndef DUK_RHI_SHADER_REFLECTION_H
#define DUK_RHI_SHADER_REFLECTION_H

#include <duk_rhi/shader.h>
#include <duk_rhi/vertex_layout.h>

#include <unordered_map>
#include <vector>

struct SpvReflectShaderModule;

namespace duk::rhi {

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

private:
    struct SpvReflectShaderModuleDeleter {
        void operator()(SpvReflectShaderModule* module) const noexcept;
    };
    std::unordered_map<ShaderModule::Bits, std::unique_ptr<SpvReflectShaderModule, SpvReflectShaderModuleDeleter>> m_stageModules;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_REFLECTION_H
