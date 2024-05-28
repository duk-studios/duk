//
// Created by rov on 12/2/2023.
//

#ifndef DUK_RENDERER_MATERIAL_HANDLER_H
#define DUK_RENDERER_MATERIAL_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_renderer/material/material_pool.h>

#include <filesystem>

namespace duk::renderer {

class MaterialHandler : public duk::resource::ResourceHandlerT<MaterialPool> {
public:
    explicit MaterialHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<Material> load(MaterialPool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_MATERIAL_HANDLER_H
