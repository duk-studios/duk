//
// Created by rov on 11/21/2023.
//

#ifndef DUK_RENDERER_MESH_BUILTINS_H
#define DUK_RENDERER_MESH_BUILTINS_H

#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/mesh/mesh_buffer.h>
#include <duk_resource/pool.h>

namespace duk::renderer {

// 10,001 - 20,000 - reserved for built-in mesh
static constexpr duk::resource::Id kQuadMeshId(10001);
static constexpr duk::resource::Id kCubeMeshId(10002);
static constexpr duk::resource::Id kSphereMeshId(10003);
static constexpr duk::resource::Id kConeMeshId(10004);

class Renderer;
class StaticMeshDataSource;

struct MeshBuiltinsCreateInfo {
    duk::resource::Pools* pools;
    MeshBufferPool* meshBufferPool;
};

class MeshBuiltins {
public:
    explicit MeshBuiltins(const MeshBuiltinsCreateInfo& meshBuiltinsCreateInfo);

    DUK_NO_DISCARD duk::resource::Handle<Mesh> quad() const;

    DUK_NO_DISCARD duk::resource::Handle<Mesh> cube() const;

    DUK_NO_DISCARD duk::resource::Handle<Mesh> sphere() const;

    DUK_NO_DISCARD duk::resource::Handle<Mesh> cone() const;

private:
    duk::resource::Handle<Mesh> m_quad;
    duk::resource::Handle<Mesh> m_cube;
    duk::resource::Handle<Mesh> m_sphere;
    duk::resource::Handle<Mesh> m_cone;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MESH_BUILTINS_H
