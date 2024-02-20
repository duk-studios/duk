//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_loader_json.h>
#include <duk_tools/file.h>

namespace duk::import {

bool SceneLoaderJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".scn";
}

std::unique_ptr<duk::scene::Scene> SceneLoaderJson::load(const std::filesystem::path& path) {

    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    auto scene = std::make_unique<duk::scene::Scene>();

    auto jsonObjects = root["objects"].GetArray();

    for (auto& jsonObject : jsonObjects) {

        auto object = scene->add_object();

        auto jsonComponents = jsonObject["components"].GetArray();

        for (auto& jsonComponent : jsonComponents) {
            duk::scene::ComponentRegistry::instance()->build_from_json(object, jsonComponent);
        }
    }

    return scene;
}

}
