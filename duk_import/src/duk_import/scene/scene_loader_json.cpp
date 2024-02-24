//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_loader_json.h>
#include <duk_tools/file.h>
#include <rapidjson/error/en.h>

namespace duk::import {

bool SceneLoaderJson::accepts(const std::filesystem::path& path) {
    return path.extension() == ".scn";
}

std::shared_ptr<duk::scene::Scene> SceneLoaderJson::load(const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    if (result.HasParseError()) {
        throw std::runtime_error(fmt::format("failed to parse scene json: {}", rapidjson::GetParseError_En(result.GetParseError())));
    }

    auto root = result.GetObject();

    auto scene = std::make_shared<duk::scene::Scene>();

    duk::json::from_json(root, *scene);

    return scene;
}

}// namespace duk::import
