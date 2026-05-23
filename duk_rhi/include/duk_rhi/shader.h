/// 18/04/2023
/// shader.h

#ifndef DUK_RHI_SHADER_H
#define DUK_RHI_SHADER_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

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

enum class BufferBindingType {
    UNIFORM_BUFFER,
    STORAGE_BUFFER
};

struct BufferMemberDescription {
    uint32_t offset;
    uint32_t size;
    uint32_t padding;
    std::string name;

    uint32_t total_size() const {
        return size + padding;
    }
};

struct BufferBindingDescription {
    BufferBindingType type;
    uint32_t size;  ///< Actual (unpadded) data size of the block in bytes.
    uint32_t stride;///< Aligned/padded stride used for dynamic offset calculations.
    std::vector<BufferMemberDescription> members;
};

enum class ImageBindingType {
    IMAGE,
    IMAGE_SAMPLER,
    STORAGE_IMAGE
};

struct ImageBindingDescription {
    ImageBindingType type;
};

struct BindingDescription {
    std::variant<ImageBindingDescription, BufferBindingDescription> binding;
    ShaderModule::Mask moduleMask;
    std::string name;
};

/// Flat ordered list of all descriptors in a shader, indexed by logical binding slot.
/// Position in the vector IS the logical index exposed to the user (e.g. via a generated enum).
/// Backend implementations decide the concrete (set, binding) numbers internally.
using ShaderBindingLayout = std::vector<BindingDescription>;

class Shader {
public:
    virtual ~Shader();

    DUK_NO_DISCARD virtual const ShaderBindingLayout& binding_layout() const = 0;

    DUK_NO_DISCARD virtual const VertexLayout& vertex_layout() const = 0;

    DUK_NO_DISCARD virtual bool is_graphics_shader() const = 0;

    DUK_NO_DISCARD virtual bool is_compute_shader() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_H
