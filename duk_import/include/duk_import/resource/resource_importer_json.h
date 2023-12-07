//
// Created by rov on 12/6/2023.
//

#ifndef DUK_IMPORT_RESOURCE_IMPORTER_JSON_H
#define DUK_IMPORT_RESOURCE_IMPORTER_JSON_H

#include <duk_import/resource/resource_importer.h>

namespace duk::import {

class ResourceImporterJson : public ResourceImporter {
public:

    DUK_NO_DISCARD bool accept(const std::filesystem::path& path) const override;

    DUK_NO_DISCARD ResourceSet load(const std::filesystem::path& path) const override;

};

}

#endif //DUK_IMPORT_RESOURCE_IMPORTER_JSON_H
