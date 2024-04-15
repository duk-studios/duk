//
// Created by rov on 12/7/2023.
//

#include <duk_engine/scene/scene_handler.h>

#include <duk_tools/file.h>

namespace duk::engine {

SceneHandler::SceneHandler()
    : ResourceHandlerT("scn") {
}

bool SceneHandler::accepts(const std::string& extension) const {
    return extension == ".scn";
}

void SceneHandler::load(ScenePool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::load_bytes(path);

    duk::serial::JsonReader reader(content);

    auto scene = std::make_shared<Scene>();

    reader.visit(*scene);

    pool->insert(id, scene);
}

}// namespace duk::engine
