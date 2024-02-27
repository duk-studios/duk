/// 19/02/2024
/// resource_importer.h

#ifndef DUK_IMPORT_RESOURCE_IMPORTER_H
#define DUK_IMPORT_RESOURCE_IMPORTER_H

#include "resource_set_importer.h"
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>
#include <filesystem>

namespace duk::import {

class ResourceImporter {
public:
    virtual ~ResourceImporter() = default;

    virtual const std::string& tag() const = 0;

    virtual void load(const duk::resource::Id& id, const std::filesystem::path& path) = 0;

    virtual void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver);

    virtual void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver);
};

template<typename DataSourceT>
class ResourceLoader {
public:
    virtual ~ResourceLoader() = default;

    virtual bool accepts(const std::filesystem::path& path) = 0;

    virtual DataSourceT load(const std::filesystem::path& path) = 0;
};

template<typename DataSourceT>
class ResourceImporterT : public ResourceImporter {
public:
    DataSourceT load(const std::filesystem::path& path);

    template<typename LoaderT, typename... Args>
    void add_loader(Args&&... args)
        requires std::is_base_of_v<ResourceLoader<DataSourceT>, LoaderT>;

private:
    std::vector<std::unique_ptr<ResourceLoader<DataSourceT>>> m_loaders;
};

template<typename DataSourceT>
DataSourceT ResourceImporterT<DataSourceT>::load(const std::filesystem::path& path) {
    for (auto& loader: m_loaders) {
        if (loader->accepts(path)) {
            return loader->load(path);
        }
    }
    throw std::runtime_error("failed to load resource: could not find suitable loader");
}

template<typename DataSourceT>
template<typename LoaderT, typename... Args>
void ResourceImporterT<DataSourceT>::add_loader(Args&&... args)
    requires std::is_base_of_v<ResourceLoader<DataSourceT>, LoaderT>
{
    m_loaders.emplace_back(std::make_unique<LoaderT>(std::forward<Args>(args)...));
}

}// namespace duk::import

#endif// DUK_IMPORT_RESOURCE_IMPORTER_H
