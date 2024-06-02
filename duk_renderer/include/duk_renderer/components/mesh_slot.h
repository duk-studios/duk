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

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::MeshSlot& slot) {
    from_json_member(json, "mesh", slot.mesh);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::MeshSlot& slot) {
    to_json_member(document, json, "mesh", slot.mesh);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MeshSlot& slot) {
    solver->solve(slot.mesh);
}

}// namespace duk::resource

#endif//DUK_RENDERER_MESH_SLOT_H
