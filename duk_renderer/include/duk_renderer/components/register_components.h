/// 03/02/2024
/// register_components.h

#ifndef DUK_RENDERER_REGISTER_COMPONENTS_H
#define DUK_RENDERER_REGISTER_COMPONENTS_H

#include <duk_scene/component_builder.h>

namespace duk::renderer {

void register_components(duk::scene::ComponentBuilder& componentBuilder);

}

#endif // DUK_RENDERER_REGISTER_COMPONENTS_H

