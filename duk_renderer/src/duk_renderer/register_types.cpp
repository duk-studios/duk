/// 03/02/2024
/// register_types.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/canvas.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/text_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/font/font_handler.h>
#include <duk_renderer/shader/shader_module_handler.h>
#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/image/image_handler.h>
#include <duk_renderer/material/material_handler.h>
#include <duk_renderer/sprite/sprite_handler.h>
#include <duk_renderer/register_types.h>
#include <duk_objects/register_types.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

void register_types() {
    duk::objects::register_types();
    duk::resource::register_handler<FontHandler>();
    duk::resource::register_handler<ShaderModuleHandler>();
    duk::resource::register_handler<ShaderPipelineHandler>();
    duk::resource::register_handler<MaterialHandler>();
    duk::resource::register_handler<ImageHandler>();
    duk::resource::register_handler<SpriteHandler>();
    duk::objects::register_component<DirectionalLight>();
    duk::objects::register_component<PointLight>();
    duk::objects::register_component<Transform>();
    duk::objects::register_component<SpriteRenderer>();
    duk::objects::register_component<MeshSlot>();
    duk::objects::register_component<MaterialSlot>();
    duk::objects::register_component<TextRenderer>();
    duk::objects::register_component<Camera>();
    duk::objects::register_component<Canvas>();
    duk::objects::register_component<CanvasTransform>();
    duk::objects::register_component<PerspectiveCamera>();
}

}// namespace duk::renderer