/// 18/04/2023
/// shader.h

#ifndef DUK_RHI_SHADER_H
#define DUK_RHI_SHADER_H

#include <duk_rhi/binding_layout.h>

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

namespace duk::rhi {

class VertexLayout;

class Shader {
public:
    virtual ~Shader();

    DUK_NO_DISCARD virtual const BindingLayout& binding_layout() const = 0;

    DUK_NO_DISCARD virtual const VertexLayout& vertex_layout() const = 0;

    DUK_NO_DISCARD virtual bool is_graphics_shader() const = 0;

    DUK_NO_DISCARD virtual bool is_compute_shader() const = 0;

    DUK_NO_DISCARD virtual duk::hash::Hash hash() const = 0;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_H
