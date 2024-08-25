/// 03/02/2024
/// register_types.cpp

#include <duk_ui/components/canvas.h>
#include <duk_ui/components/text.h>
#include <duk_ui/font/font_handler.h>
#include <duk_ui/systems/canvas_system.h>
#include <duk_ui/systems/text_system.h>
#include <duk_renderer/register_types.h>

#include <duk_serial/json/serializer.h>
#include <duk_objects/object_solver.h>
#include <duk_system/system_solver.h>

namespace duk::ui {

void register_types() {
    duk::renderer::register_types();
    duk::resource::register_handler<FontHandler>();
    duk::system::register_system<CanvasUpdateSystem>();
    duk::system::register_system<TextUpdateSystem>();
    duk::objects::register_component<Text>();
    duk::objects::register_component<Canvas>();
    duk::objects::register_component<CanvasTransform>();
    duk::objects::register_component<CanvasTransformRect>();
}

}// namespace duk::ui