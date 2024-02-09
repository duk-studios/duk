/// 03/02/2024
/// register_components.cpp

#include <duk_renderer/components/register_components.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/mesh_renderer.h>

namespace duk::renderer {

void register_components(duk::scene::ComponentBuilder* componentBuilder) {
    componentBuilder->add<DirectionalLight>("DirectionalLight");
    componentBuilder->add<PointLight>("PointLight");
    componentBuilder->add<Position3D>("Position3D");
    componentBuilder->add<Position2D>("Position2D");
    componentBuilder->add<Rotation3D>("Rotation3D");
    componentBuilder->add<Rotation2D>("Rotation2D");
    componentBuilder->add<Scale3D>("Scale3D");
    componentBuilder->add<Scale2D>("Scale2D");
    componentBuilder->add<SpriteRenderer>("SpriteRenderer");
    componentBuilder->add<MeshRenderer>("MeshRenderer");
}

}