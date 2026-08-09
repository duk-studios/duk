//
// Created by rov on 09/08/2026.
//

#ifndef DUK_RHI_BINDING_LAYOUT_H
#define DUK_RHI_BINDING_LAYOUT_H

#include <duk_rhi/shader_module.h>

#include <string>
#include <vector>
#include <optional>

namespace duk::rhi {

enum class BindingType {
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    IMAGE,
    SAMPLER_IMAGE,
    STORAGE_IMAGE
};

struct BindingMemberDescription {
    uint32_t offset;
    uint32_t size;
    uint32_t padding;
    std::string name;
    std::string typeName;

    uint32_t total_size() const {
        return size + padding;
    }
};

struct BindingDescription {
    BindingType type;
    ShaderModule::Mask moduleMask;
    std::string name;
    uint32_t size;
    uint32_t stride;
    std::vector<BindingMemberDescription> members;
};

using BindingLayout = std::vector<BindingDescription>;

std::optional<BindingDescription> find_binding(const BindingLayout& layout, const std::string_view& name);

}

#endif //DUK_RHI_BINDING_LAYOUT_H
