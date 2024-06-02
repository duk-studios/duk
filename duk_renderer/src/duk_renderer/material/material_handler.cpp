//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_handler.h>

#include <duk_serial/json/serializer.h>

#include <duk_tools/file.h>

namespace duk::renderer {

MaterialHandler::MaterialHandler()
    : ResourceHandlerT("mat") {
}

bool MaterialHandler::accepts(const std::string& extension) const {
    return extension == ".mat";
}

duk::resource::Handle<Material> MaterialHandler::load(MaterialPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("material at ({}) does not exist", path.string()));
    }

    if (path.extension() != ".mat") {
        throw std::runtime_error(fmt::format("tried to load material at ({}), which does not have the correct extension", path.string()));
    }

    auto content = duk::tools::load_text(path);

    auto materialData = duk::serial::read_json<MaterialData>(content);

    return pool->create(id, std::move(materialData));
}
}// namespace duk::renderer
