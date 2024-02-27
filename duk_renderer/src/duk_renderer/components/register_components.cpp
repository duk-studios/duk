/// 03/02/2024
/// register_components.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/register_components.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

void register_components() {
    duk::scene::register_component<DirectionalLight>();
    duk::scene::register_component<PointLight>();
    duk::scene::register_component<Position3D>();
    duk::scene::register_component<Position2D>();
    duk::scene::register_component<Rotation3D>();
    duk::scene::register_component<Rotation2D>();
    duk::scene::register_component<Scale3D>();
    duk::scene::register_component<Scale2D>();
    duk::scene::register_component<SpriteRenderer>();
    duk::scene::register_component<MeshRenderer>();
    duk::scene::register_component<Camera>();
    duk::scene::register_component<PerspectiveCamera>();
}

}// namespace duk::renderer