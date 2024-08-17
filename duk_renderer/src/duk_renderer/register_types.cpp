/// 03/02/2024
/// register_types.cpp

#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/material_slot.h>
#include <duk_renderer/components/mesh_slot.h>
#include <duk_renderer/systems/camera_system.h>
#include <duk_renderer/systems/sprite_system.h>
#include <duk_renderer/systems/transform_system.h>
#include <duk_renderer/shader/shader_module_handler.h>
#include <duk_renderer/shader/shader_pipeline_handler.h>
#include <duk_renderer/image/image_handler.h>
#include <duk_renderer/material/material_handler.h>
#include <duk_renderer/sprite/sprite_handler.h>
#include <duk_renderer/register_types.h>
#include <duk_objects/register_types.h>
#include <duk_objects/object_solver.h>
#include <duk_system/system_solver.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

void register_types() {
    duk::objects::register_types();
    duk::resource::register_handler<ShaderModuleHandler>();
    duk::resource::register_handler<ShaderPipelineHandler>();
    duk::resource::register_handler<MaterialHandler>();
    duk::resource::register_handler<ImageHandler>();
    duk::resource::register_handler<SpriteHandler>();
    duk::system::register_system<CameraUpdateSystem>();
    duk::system::register_system<TransformUpdateSystem>();
    duk::system::register_system<SpriteUpdateSystem>();
    duk::objects::register_component<DirectionalLight>();
    duk::objects::register_component<PointLight>();
    duk::objects::register_component<Transform>();
    duk::objects::register_component<Matrices>();
    duk::objects::register_component<SpriteRenderer>();
    duk::objects::register_component<MeshSlot>();
    duk::objects::register_component<MaterialSlot>();
    duk::objects::register_component<Camera>();
    duk::objects::register_component<PerspectiveCamera>();
}

}// namespace duk::renderer