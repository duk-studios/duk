//
// Created by Ricardo on 23/04/2024.
//

#include <duk_renderer/sprite/sprite_handler.h>

#include <duk_serial/json/serializer.h>

#include <duk_tools/file.h>

namespace duk::renderer {

SpriteHandler::SpriteHandler()
    : TextHandlerT("spt") {
}

bool SpriteHandler::accepts(const std::string& extension) const {
    return extension == ".spt";
}

std::shared_ptr<Sprite> SpriteHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    const auto spriteAtlasData = duk::serial::read_json<SpriteAtlasData>(text);

    SpriteAtlasCreateInfo spriteAtlasCreateInfo = {};
    spriteAtlasCreateInfo.spriteAtlasData = &spriteAtlasData;

    return std::make_shared<Sprite>(spriteAtlasCreateInfo);
}

}// namespace duk::renderer
