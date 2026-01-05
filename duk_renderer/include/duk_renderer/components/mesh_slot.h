//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_RENDERER_MESH_SLOT_H
#define DUK_RENDERER_MESH_SLOT_H

#include <duk_renderer/mesh/mesh.h>

namespace duk::renderer {

struct MeshSlot {
    MeshResource mesh;
};

}// namespace duk::renderer

namespace duk::type {
// clang-format off
template<>
struct Type<duk::renderer::MeshSlot> : Class<duk::renderer::MeshSlot,
    Member<"mesh", &duk::renderer::MeshSlot::mesh>> {
};

// clang-format on
}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MeshSlot& slot) {
    solver->solve(slot.mesh);
}

}// namespace duk::resource

#endif//DUK_RENDERER_MESH_SLOT_H
