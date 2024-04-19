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

template<>
inline void from_json<duk::renderer::MeshRenderer>(const rapidjson::Value& json, duk::renderer::MeshRenderer& meshRenderer) {
    from_json_member(json, "mesh", meshRenderer.mesh);
    from_json_member(json, "material", meshRenderer.material);
}

template<>
inline void to_json<duk::renderer::MeshRenderer>(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::MeshRenderer& meshRenderer) {
    to_json_member(document, json, "mesh", meshRenderer.mesh);
    to_json_member(document, json, "material", meshRenderer.material);
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
