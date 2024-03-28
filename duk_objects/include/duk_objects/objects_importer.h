//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_OBJECTS_IMPORTER_H
#define DUK_OBJECTS_IMPORTER_H

#include <duk_objects/objects_pool.h>

#include <duk_resource/importer.h>

namespace duk::objects {

struct ObjectsImporterCreateInfo {
    ObjectsPool* objectsPool;
};

class ObjectsImporter : public duk::resource::ResourceImporter {
public:
    ObjectsImporter(const ObjectsImporterCreateInfo& objectsImporterCreateInfo);

    ~ObjectsImporter() override;

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    ObjectsPool* m_objectsPool;
};

}// namespace duk::objects

#endif//DUK_OBJECTS_IMPORTER_H
