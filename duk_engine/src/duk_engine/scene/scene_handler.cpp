//
// Created by rov on 12/7/2023.
//

#include <duk_engine/scene/scene_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::engine {

SceneHandler::SceneHandler()
    : TextHandlerT("scn") {
}

bool SceneHandler::accepts(const std::string& extension) const {
    return extension == ".scn";
}

std::shared_ptr<Scene> SceneHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    auto scene = std::make_shared<Scene>();
    duk::serial::read_json(text, *scene);
    return scene;
}

}// namespace duk::engine
