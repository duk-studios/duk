#ifndef DUK_GENERATED_FULLSCREEN_SHADER_DATA_SOURCE_H
#define DUK_GENERATED_FULLSCREEN_SHADER_DATA_SOURCE_H

#include <duk_rhi/shader_data_source.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace duk::renderer {

class FullscreenShaderDataSource : public duk::rhi::ShaderDataSource {
public:
    enum class Binding : uint32_t {
        U_TEXTURE = 0,
        U_PROPERTIES = 1,
    };

    duk::rhi::ShaderModule::Mask module_mask() const override;

    const std::vector<uint8_t>& shader_module_spir_v_code(duk::rhi::ShaderModule::Bits type) const override;

    const std::unordered_map<duk::rhi::ShaderModule::Bits, std::vector<uint8_t>>& shader_modules() const override;

    const duk::rhi::ShaderBindingLayout& binding_layout() const override;

    const duk::rhi::VertexLayout& vertex_layout() const override;

protected:
    duk::hash::Hash calculate_hash() const override;
};

}// namespace duk::renderer

#endif// DUK_GENERATED_FULLSCREEN_SHADER_DATA_SOURCE_H
