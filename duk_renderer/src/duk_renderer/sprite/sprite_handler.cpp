//
// Created by Ricardo on 23/04/2024.
//

#include <duk_renderer/sprite/sprite_handler.h>

#include <duk_serial/json/serializer.h>

#include <duk_tools/file.h>

namespace duk::renderer {

SpriteHandler::SpriteHandler()
    : TextResourceHandlerT("spt") {
}

bool SpriteHandler::accepts(const std::string& extension) const {
    return extension == ".spt";
}

duk::resource::Handle<Sprite> SpriteHandler::load_from_text(SpritePool* pool, const resource::Id& id, const std::string_view& text) {
    auto spriteAtlasData = duk::serial::read_json<SpriteAtlasData>(text);

    SpriteAtlasCreateInfo spriteAtlasCreateInfo = {};
    spriteAtlasCreateInfo.spriteAtlasData = &spriteAtlasData;

    auto spriteAtlas = std::make_shared<Sprite>(spriteAtlasCreateInfo);

    return pool->insert(id, spriteAtlas);
}

}// namespace duk::renderer
