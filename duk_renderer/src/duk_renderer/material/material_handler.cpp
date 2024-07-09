//
// Created by rov on 12/2/2023.
//

#include <duk_renderer/material/material_handler.h>
#include <duk_renderer/renderer.h>

#include <duk_serial/json/serializer.h>

namespace duk::renderer {

MaterialHandler::MaterialHandler()
    : TextHandlerT("mat") {
}

bool MaterialHandler::accepts(const std::string& extension) const {
    return extension == ".mat";
}

std::shared_ptr<Material> MaterialHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    auto materialData = duk::serial::read_json<MaterialData>(text);

    const auto renderer = globals->get<Renderer>();

    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.rhi = renderer->rhi();
    materialCreateInfo.commandQueue = renderer->main_command_queue();
    materialCreateInfo.materialData = std::move(materialData);

    return std::make_shared<Material>(materialCreateInfo);
}

}// namespace duk::renderer
