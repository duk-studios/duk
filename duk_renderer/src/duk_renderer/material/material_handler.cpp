//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

MaterialHandler::MaterialHandler()
    : TextHandlerT("mat") {
}

bool MaterialHandler::accepts(const std::string& extension) const {
    return extension == ".mat";
}

duk::resource::Handle<Material> MaterialHandler::load_from_text(MaterialPool* pool, const resource::Id& id, const std::string_view& text) {
    auto materialData = duk::serial::read_json<MaterialData>(text);

    return pool->create(id, std::move(materialData));
}

}// namespace duk::renderer
