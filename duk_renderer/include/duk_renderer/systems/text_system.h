//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_RENDERER_TEXT_SYSTEM_H
#define DUK_RENDERER_TEXT_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_renderer/components/text_renderer.h>

namespace duk::renderer {

class TextUpdateSystem : public duk::system::System {
public:
    using TextRendererEnterEvent = objects::ComponentEnterEventT<TextRenderer>;

    void attach();

    void update() override;

    void receive(const TextRendererEnterEvent& event);
};

}// namespace duk::renderer

#endif//DUK_RENDERER_TEXT_SYSTEM_H
