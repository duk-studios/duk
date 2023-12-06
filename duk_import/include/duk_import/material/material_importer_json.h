//
// Created by rov on 12/4/2023.
//

#ifndef DUK_IMPORT_MATERIAL_IMPORTER_JSON_H
#define DUK_IMPORT_MATERIAL_IMPORTER_JSON_H

#include <duk_import/material/material_importer.h>

namespace duk::import {

class MaterialImporterJson : public MaterialImporter {
public:

    MaterialImporterJson(const duk::renderer::ImagePool* imagePool);

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::renderer::MaterialDataSource> load(const std::filesystem::path& path) override;
private:
    const duk::renderer::ImagePool* m_imagePool;

};

}


#endif //DUK_IMPORT_MATERIAL_IMPORTER_JSON_H
