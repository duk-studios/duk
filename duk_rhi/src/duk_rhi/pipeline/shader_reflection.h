//
// Created by Ricardo on 05/05/2024.
//

#ifndef DUK_RHI_SHADER_REFLECTION_H
#define DUK_RHI_SHADER_REFLECTION_H

#include <duk_rhi/descriptor_set.h>
#include <duk_rhi/vertex_layout.h>

namespace duk::rhi {

void reflect_descriptor_sets(const uint8_t* data, size_t size, ShaderModule::Bits shaderModuleBit, std::vector<DescriptorSetDescription>& descriptorSetDescriptions);

void reflect_vertex_layout(const uint8_t* data, size_t size, VertexLayout& vertexLayout);

} // duk::rhi

#endif //DUK_RHI_SHADER_REFLECTION_H
