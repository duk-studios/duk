//
// Created by Ricardo on 28/03/2024.
//

#include <duk_objects/objects_handler.h>

#include <duk_tools/file.h>

namespace duk::objects {

ObjectsHandler::ObjectsHandler(const ObjectsHandlerCreateInfo& objectsHandlerCreateInfo)
    : m_objectsPool(objectsHandlerCreateInfo.objectsPool) {
}

ObjectsHandler::~ObjectsHandler() {
}

const std::string& ObjectsHandler::tag() const {
    static const std::string tag = "obj";
    return tag;
}

void ObjectsHandler::load(const duk::resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    duk::serial::JsonReader reader(content.c_str());

    auto objects = std::make_shared<Objects>();

    reader.visit(*objects);

    m_objectsPool->insert(id, objects);
}

void ObjectsHandler::solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) {
    auto objects = m_objectsPool->find(id);
    if (!objects) {
        throw std::logic_error("tried to solve dependencies of objects that were not loaded");
    }
    dependencySolver.solve(*objects);
}

void ObjectsHandler::solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) {
    auto objects = m_objectsPool->find(id);
    if (!objects) {
        throw std::logic_error("tried to solve references of objects that were not loaded");
    }
    referenceSolver.solve(*objects);
}

}// namespace duk::objects