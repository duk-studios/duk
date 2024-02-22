//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_JSON_H
#define DUK_IMPORT_SCENE_IMPORTER_JSON_H

#include <duk_import/scene/scene_importer.h>

namespace duk::import {

class SceneLoaderJson : public ResourceLoader<duk::scene::Scene> {
public:

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::scene::Scene> load(const std::filesystem::path& path) override;

};

}

#endif //DUK_IMPORT_SCENE_IMPORTER_JSON_H
