/// 13/11/2023
/// importer.h

#ifndef DUK_IMPORT_IMPORTER_H
#define DUK_IMPORT_IMPORTER_H

#include <duk_import/resource_importer.h>
#include <duk_import/resource_set_importer.h>

namespace duk::import {

// 1,000,000 reserved for built-in resources
static constexpr duk::resource::Id kMaxBuiltInResourceId(1000000);

struct ImporterCreateInfo {
    duk::resource::Pools* pools;
};

class Importer {
public:
    explicit Importer(const ImporterCreateInfo& importerCreateInfo);

    ~Importer();

    ResourceImporter* add_resource_importer(std::unique_ptr<ResourceImporter> resourceImporter);

    template<typename T, typename... Args>
    T* add_resource_importer(Args&&... args);

    void load_resources(const std::filesystem::path& path);

    void load_resource(duk::resource::Id id);

    duk::resource::Id find_id(const std::string& alias);

    ResourceImporter* get_importer(const std::string& tag);

    template<typename T>
    T* get_importer_as(const std::string& tag);

private:
    duk::resource::Pools* m_pools;
    ResourceSetImporter m_resourceSetImporter;
    std::unordered_map<std::string, std::unique_ptr<ResourceImporter>> m_resourceImporters;
    ResourceSet m_resourceSet;
};

template<typename T, typename... Args>
T* Importer::add_resource_importer(Args&&... args) {
    return dynamic_cast<T*>(add_resource_importer(std::make_unique<T>(std::forward<Args>(args)...)));
}

template<typename T>
T* Importer::get_importer_as(const std::string& tag) {
    auto importer = get_importer(tag);
    if (importer->tag() != tag) {
        return nullptr;
    }
    return dynamic_cast<T*>(importer);
}

}// namespace duk::import

#endif// DUK_IMPORT_IMPORTER_H
