//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_loader_json.h>
#include <duk_tools/file.h>

namespace duk::import {

bool SceneLoaderJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".scn";
}

std::shared_ptr<duk::scene::Scene> SceneLoaderJson::load(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto scene = std::make_shared<duk::scene::Scene>();

    reader.visit(*scene);

    return scene;
}

}// namespace duk::import
