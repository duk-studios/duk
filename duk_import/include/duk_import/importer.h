/// 13/11/2023
/// importer.h

#ifndef DUK_IMPORT_IMPORTER_H
#define DUK_IMPORT_IMPORTER_H

#include <duk_import/image/image_importer.h>
#include <duk_import/material/material_importer.h>
#include "resource_set_importer.h"
#include <duk_import/scene/scene_importer.h>
#include <duk_import/resource_importer.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/pools/image_pool.h>
#include <duk_renderer/pools/material_pool.h>

namespace duk::import {

struct ImporterCreateInfo {
    duk::renderer::Renderer* renderer;
    duk::resource::Pools* pools;
};

class Importer {
public:

    explicit Importer(const ImporterCreateInfo& importerCreateInfo);

    ~Importer();

    void load_resources(const std::filesystem::path& path);

    void load_resource(duk::resource::Id id);

    duk::resource::Id find_id_from_alias(const std::string& alias);

    ResourceImporter* importer_for_resource_type(const std::string& resourceType);

private:
    duk::resource::Pools* m_pools;
    ResourceSetImporter m_resourceSetImporter;
    std::unordered_map<std::string, std::unique_ptr<ResourceImporter>> m_resourceImporters;
    ResourceSet m_resourceSet;
};

}

#endif // DUK_IMPORT_IMPORTER_H

