/// 18/04/2023
/// shader.h

#ifndef DUK_RHI_SHADER_H
#define DUK_RHI_SHADER_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <cstdint>
#include <string>

namespace duk::rhi {

class VertexLayout;

struct ShaderModule {
    enum Bits : uint32_t {
        VERTEX = 1 << 0,
        TESSELLATION_CONTROL = 1 << 1,
        TESSELLATION_EVALUATION = 1 << 2,
        GEOMETRY = 1 << 3,
        FRAGMENT = 1 << 4,
        COMPUTE = 1 << 5,
    };

    static constexpr uint32_t kCount = 6;
    using Mask = uint32_t;
};

enum class DescriptorType {
    UNDEFINED = 0,
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    IMAGE,
    IMAGE_SAMPLER,
    STORAGE_IMAGE
};

struct DescriptorMemberDescription {
    uint32_t offset;
    uint32_t size;
    uint32_t padding;
    std::string name;

    uint32_t total_size() const {
        return size + padding;
    }
};

struct DescriptorDescription {
    DescriptorType type;
    ShaderModule::Mask moduleMask;
    std::string name;
    uint32_t stride;
    std::vector<DescriptorMemberDescription> members;
    //
    // uint32_t total_size() const {
    //     const auto& member = members.back();
    //     return member.total_size() + member.offset;
    // }
};

struct DescriptorSetDescription {
    std::vector<DescriptorDescription> bindings;
};

class Shader {
public:
    virtual ~Shader();

    DUK_NO_DISCARD virtual const DescriptorSetDescription& descriptor_set_description(uint32_t set = 0) const = 0;

    DUK_NO_DISCARD virtual const VertexLayout& vertex_layout() const = 0;

    DUK_NO_DISCARD virtual bool is_graphics_shader() const = 0;

    DUK_NO_DISCARD virtual bool is_compute_shader() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;
};


}// namespace duk::rhi

#endif// DUK_RHI_SHADER_H
