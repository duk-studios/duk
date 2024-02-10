/// 20/08/2023
/// mesh_renderer.h

#ifndef DUK_RENDERER_MESH_RENDERER_H
#define DUK_RENDERER_MESH_RENDERER_H

#include <duk_renderer/pools/mesh_pool.h>
#include <duk_renderer/resources/materials/mesh_material.h>

namespace duk::renderer {

struct MeshRenderer {
    MeshResource mesh;
    MeshMaterialResource material;
};

}

namespace duk::json {

template<>
inline void from_json<duk::renderer::MeshRenderer>(const rapidjson::Value& jsonObject, duk::renderer::MeshRenderer& object) {
    object.mesh = from_json<duk::resource::Id>(jsonObject["mesh"]);
    object.material = from_json<duk::resource::Id>(jsonObject["material"]);
}

}

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MeshRenderer& meshRenderer) {
    solver->solve(meshRenderer.mesh);
    solver->solve(meshRenderer.material);
}

}

#endif // DUK_RENDERER_MESH_RENDERER_H
