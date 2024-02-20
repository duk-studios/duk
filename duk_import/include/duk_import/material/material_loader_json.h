//
// Created by rov on 12/4/2023.
//

#ifndef DUK_IMPORT_MATERIAL_IMPORTER_JSON_H
#define DUK_IMPORT_MATERIAL_IMPORTER_JSON_H

#include <duk_import/material/material_importer.h>

namespace duk::import {

class MaterialLoaderJson : public ResourceLoader<duk::renderer::MaterialDataSource> {
public:
    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::renderer::MaterialDataSource> load(const std::filesystem::path& path) override;

};

}


#endif //DUK_IMPORT_MATERIAL_IMPORTER_JSON_H
