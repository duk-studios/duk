//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_JSON_H
#define DUK_IMPORT_SCENE_IMPORTER_JSON_H

#include <duk_import/scene/scene_importer.h>
#include "component_json_parser.h"

#include <rapidjson/document.h>

#include <functional>
#include <span>
#include <set>

namespace duk::import {

struct SceneImporterJsonCreateInfo {
    ComponentJsonParser* componentParser;
};

class SceneImporterJson : public SceneImporter {
public:

    explicit SceneImporterJson(const SceneImporterJsonCreateInfo& sceneImporterJsonCreateInfo);

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::scene::Scene> load(const std::filesystem::path& path) override;

private:
    ComponentJsonParser* m_componentParser;
};

}

#endif //DUK_IMPORT_SCENE_IMPORTER_JSON_H
