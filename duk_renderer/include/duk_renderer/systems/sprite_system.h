//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_RENDERER_SPRITE_SYSTEM_H
#define DUK_RENDERER_SPRITE_SYSTEM_H

#include <duk_objects/events.h>
#include <duk_renderer/components/sprite_renderer.h>
#include <duk_system/system.h>

namespace duk::renderer {

class SpriteUpdateSystem : public duk::system::System {
public:
    using SpriteRendererEnterEvent = objects::ComponentEnterEventT<duk::renderer::SpriteRenderer>;
    using SpriteRendererExitEvent = objects::ComponentExitEventT<duk::renderer::SpriteRenderer>;

    void attach() override;

    void update() override;

    void receive(const SpriteRendererEnterEvent& event);

    void receive(const SpriteRendererExitEvent& event);
};

}// namespace duk::renderer

#endif//DUK_RENDERER_SPRITE_SYSTEM_H
