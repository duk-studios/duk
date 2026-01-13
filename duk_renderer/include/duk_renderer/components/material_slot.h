//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_RENDERER_MATERIAL_SLOT_H
#define DUK_RENDERER_MATERIAL_SLOT_H

#include <duk_renderer/material/material.h>

namespace duk::renderer {

struct MaterialSlot {
    MaterialResource material;
};

}// namespace duk::renderer

namespace duk::type {
// clang-format off
template<>
struct Type<duk::renderer::MaterialSlot> : Class<duk::renderer::MaterialSlot,
    Member<"material", &duk::renderer::MaterialSlot::material>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_RENDERER_MATERIAL_SLOT_H
