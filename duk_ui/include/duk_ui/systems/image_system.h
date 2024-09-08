//
// Created by Ricardo on 26/08/2024.
//

#ifndef DUK_UI_IMAGE_SYSTEM_H
#define DUK_UI_IMAGE_SYSTEM_H

#include <duk_system/system.h>

namespace duk::ui {

class ImageSystem : public duk::system::System {
public:
    void update() override;

private:
    void update_material(const duk::objects::Component<Image>& image);

    void update_mesh(const duk::objects::Component<CanvasTransform>& canvasTransform, const duk::objects::Component<Image>& image);

private:
    std::unordered_map<uint64_t, duk::resource::Handle<duk::renderer::Material>> m_materials;
};

}// namespace duk::ui

#endif//DUK_UI_IMAGE_SYSTEM_H
