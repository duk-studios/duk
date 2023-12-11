//
// Created by rov on 12/7/2023.
//

#ifndef DUK_IMPORT_SCENE_IMPORTER_H
#define DUK_IMPORT_SCENE_IMPORTER_H

#include <duk_scene/scene.h>
#include <filesystem>

namespace duk::import {

class SceneImporter {
public:

    virtual ~SceneImporter();

    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual std::unique_ptr<duk::scene::Scene> load(const std::filesystem::path& path) = 0;
};

}

#endif //DUK_IMPORT_SCENE_IMPORTER_H
