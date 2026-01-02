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

template<>
struct Type<duk::renderer::MaterialSlot> : Class<duk::renderer::MaterialSlot,
    Member<"material", &duk::renderer::MaterialSlot::material>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MaterialSlot& slot) {
    solver->solve(slot.material);
}

}// namespace duk::resource

#endif//DUK_RENDERER_MATERIAL_SLOT_H
