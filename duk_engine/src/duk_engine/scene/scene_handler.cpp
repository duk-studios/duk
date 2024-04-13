//
// Created by rov on 12/7/2023.
//

#include <duk_engine/scene/scene_handler.h>

#include <duk_tools/file.h>

namespace duk::engine {

SceneHandler::SceneHandler() :
    ResourceHandlerT("scn") {

}

void SceneHandler::load(ScenePool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto scene = std::make_shared<Scene>();

    reader.visit(*scene);

    pool->insert(id, scene);
}

}// namespace duk::engine
