//
// Created by Ricardo on 28/03/2024.
//

#include <duk_objects/objects_handler.h>

#include <duk_tools/file.h>

namespace duk::objects {

ObjectsHandler::ObjectsHandler() : ResourceHandlerT("obj"){
}

void ObjectsHandler::load(ObjectsPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto objects = std::make_shared<Objects>();

    reader.visit(*objects);

    pool->insert(id, objects);
}
}// namespace duk::objects