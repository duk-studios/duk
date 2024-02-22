/// 03/02/2024
/// register_components.cpp

#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/register_components.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

void register_components() {
    auto componentRegistry = duk::scene::ComponentRegistry::instance(true);
    componentRegistry->add<DirectionalLight>("DirectionalLight");
    componentRegistry->add<PointLight>("PointLight");
    componentRegistry->add<Position3D>("Position3D");
    componentRegistry->add<Position2D>("Position2D");
    componentRegistry->add<Rotation3D>("Rotation3D");
    componentRegistry->add<Rotation2D>("Rotation2D");
    componentRegistry->add<Scale3D>("Scale3D");
    componentRegistry->add<Scale2D>("Scale2D");
    componentRegistry->add<SpriteRenderer>("SpriteRenderer");
    componentRegistry->add<MeshRenderer>("MeshRenderer");
}

}// namespace duk::renderer