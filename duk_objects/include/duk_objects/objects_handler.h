//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_OBJECTS_HANDLER_H
#define DUK_OBJECTS_HANDLER_H

#include <duk_objects/objects_pool.h>

#include <duk_resource/handler.h>

namespace duk::objects {

struct ObjectsHandlerCreateInfo {
    ObjectsPool* objectsPool;
};

class ObjectsHandler : public duk::resource::ResourceHandler {
public:
    ObjectsHandler(const ObjectsHandlerCreateInfo& objectsHandlerCreateInfo);

    ~ObjectsHandler() override;

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    ObjectsPool* m_objectsPool;
};

}// namespace duk::objects

#endif//DUK_OBJECTS_HANDLER_H
