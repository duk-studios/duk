/// 20/08/2023
/// mesh_renderer.h

#ifndef DUK_RENDERER_MESH_RENDERER_H
#define DUK_RENDERER_MESH_RENDERER_H

#include <duk_renderer/material/material.h>
#include <duk_renderer/mesh/mesh_pool.h>

namespace duk::renderer {

struct MeshRenderer {
    MeshResource mesh;
    MaterialResource material;
};

}// namespace duk::renderer

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::renderer::MeshRenderer& meshRenderer) {
    visitor->template visit_member<duk::resource::Resource>(meshRenderer.mesh, MemberDescription("mesh"));
    visitor->template visit_member<duk::resource::Resource>(meshRenderer.material, MemberDescription("material"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MeshRenderer& meshRenderer) {
    solver->solve(meshRenderer.mesh);
    solver->solve(meshRenderer.material);
}

}// namespace duk::resource

#endif// DUK_RENDERER_MESH_RENDERER_H
