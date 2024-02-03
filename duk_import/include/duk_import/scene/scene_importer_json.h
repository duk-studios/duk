//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_JSON_H
#define DUK_IMPORT_SCENE_IMPORTER_JSON_H

#include <duk_import/scene/scene_importer.h>
#include <duk_scene/component_builder.h>

#include <functional>
#include <span>
#include <set>

namespace duk::import {

struct SceneImporterJsonCreateInfo {
    duk::scene::ComponentBuilder* componentBuilder;
};

class SceneImporterJson : public SceneImporter {
public:

    explicit SceneImporterJson(const SceneImporterJsonCreateInfo& sceneImporterJsonCreateInfo);

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::scene::Scene> load(const std::filesystem::path& path) override;

private:
    duk::scene::ComponentBuilder* m_componentBuilder;
};

}

#endif //DUK_IMPORT_SCENE_IMPORTER_JSON_H
