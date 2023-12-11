//
// Created by rov on 12/7/2023.
//

#include <duk_import/scene/scene_importer_json.h>
#include <duk_tools/file.h>
#include <rapidjson/document.h>
#include <regex>

namespace duk::import {

SceneImporterJson::SceneImporterJson(const SceneImporterJsonCreateInfo& sceneImporterJsonCreateInfo) :
    m_componentParser(sceneImporterJsonCreateInfo.componentParser) {

}

SceneImporter::~SceneImporter() = default;

bool SceneImporterJson::accepts(const std::filesystem::path& path) {
    std::regex pattern("\\.scn\\.json$");
    auto filename = path.filename().string();
    return std::regex_search(filename, pattern);
}

std::unique_ptr<duk::scene::Scene> SceneImporterJson::load(const std::filesystem::path& path) {

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
            m_componentParser->parse(object, jsonComponent);
        }
    }

    return scene;
}

}
