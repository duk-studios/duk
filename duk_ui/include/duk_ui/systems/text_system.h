//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_UI_TEXT_SYSTEM_H
#define DUK_UI_TEXT_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_ui/components/text.h>
#include <duk_renderer/mesh/dynamic_mesh.h>

namespace duk::ui {

class TextUpdateSystem : public duk::system::System {
public:
    using TextRendererEnterEvent = objects::ComponentEnterEventT<Text>;

    void attach();

    void update() override;

    void receive(const TextRendererEnterEvent& event);

private:
    void update_text(const duk::objects::Component<Text>& text);

    void update_text_material(const duk::objects::Component<Text>& text, const FontAtlas* atlas);

    void update_text_mesh(const duk::objects::Component<Text>& text, const FontAtlas* atlas);

    duk::renderer::MeshBufferPool* m_meshBufferPool = nullptr;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<duk::renderer::DynamicMesh>> m_meshes;
    std::unordered_map<duk::hash::Hash, std::shared_ptr<duk::renderer::Material>> m_materials;
};

}// namespace duk::ui

#endif//DUK_UI_TEXT_SYSTEM_H
