/// 03/02/2024
/// register_types.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/canvas.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/mesh_renderer.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/text_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/register_types.h>

namespace duk::renderer {

void register_types() {
    duk::objects::register_component<DirectionalLight>();
    duk::objects::register_component<PointLight>();
    duk::objects::register_component<Transform>();
    duk::objects::register_component<SpriteRenderer>();
    duk::objects::register_component<MeshRenderer>();
    duk::objects::register_component<TextRenderer>();
    duk::objects::register_component<Camera>();
    duk::objects::register_component<Canvas>();
    duk::objects::register_component<CanvasTransform>();
    duk::objects::register_component<PerspectiveCamera>();
}

}// namespace duk::renderer