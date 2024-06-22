//
// Created by rov on 12/2/2023.
//

#ifndef DUK_RENDERER_MATERIAL_HANDLER_H
#define DUK_RENDERER_MATERIAL_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_renderer/material/material_pool.h>

#include <filesystem>

namespace duk::renderer {

class MaterialHandler : public duk::resource::TextResourceHandlerT<MaterialPool> {
public:
    explicit MaterialHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<Material> load_from_text(MaterialPool* pool, const resource::Id& id, const std::string_view& text) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MATERIAL_HANDLER_H
