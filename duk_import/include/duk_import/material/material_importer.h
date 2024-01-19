//
// Created by rov on 12/2/2023.
//

#ifndef DUK_MATERIAL_IMPORTER_H
#define DUK_MATERIAL_IMPORTER_H

#include <duk_renderer/resources/materials/phong/phong_material.h>
#include <duk_renderer/resources/materials/color/color_material.h>

#include <filesystem>

class MaterialImporter {
public:
    virtual ~MaterialImporter();

    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual std::unique_ptr<duk::renderer::MaterialDataSource> load(const std::filesystem::path& path) = 0;

};


#endif //DUK_MATERIAL_IMPORTER_H
