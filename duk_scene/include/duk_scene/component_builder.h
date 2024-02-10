/// 03/02/2024
/// component_registry.h

#ifndef DUK_SCENE_COMPONENT_REGISTRY_H
#define DUK_SCENE_COMPONENT_REGISTRY_H

#include <duk_scene/scene.h>
#include <duk_json/types.h>
#include <duk_resource/solver/reference_solver.h>

#include <string>
#include <unordered_map>
#include <memory>

namespace duk::scene {

struct ComponentBuilderCreateInfo {
    duk::resource::ReferenceSolver* solver;
};

class ComponentBuilder {
private:

    class Builder {
    public:
        virtual ~Builder() = default;

        virtual void build(Object& object, const rapidjson::Value& jsonObject) = 0;
    };

    template<typename T>
    class BuilderT : public Builder {
    public:
        BuilderT(duk::resource::ReferenceSolver* solver);

        void build(Object& object, const rapidjson::Value& jsonObject) override;
    private:
        duk::resource::ReferenceSolver* m_solver;
    };

public:

    explicit ComponentBuilder(const ComponentBuilderCreateInfo& componentBuilderCreateInfo);

    template<typename T>
    void add(const std::string& typeName);

    void build(Object& object, const rapidjson::Value& jsonObject);

private:
    std::unordered_map<std::string, std::unique_ptr<Builder>> m_componentBuilders;
    duk::resource::ReferenceSolver* m_solver;
};

template<typename T>
ComponentBuilder::BuilderT<T>::BuilderT(duk::resource::ReferenceSolver* solver) :
    m_solver(solver) {

}

template<typename T>
void ComponentBuilder::BuilderT<T>::build(Object& object, const rapidjson::Value& jsonObject) {
    auto component = json::from_json<T>(jsonObject);
    m_solver->solve(component);
    object.add<T>(component);
}

template<typename T>
void ComponentBuilder::add(const std::string& typeName) {
    assert(m_componentBuilders.find(typeName) == m_componentBuilders.end() && "Component is already registered!");
    m_componentBuilders.emplace(typeName, std::make_unique<BuilderT<T>>(m_solver));
}

}

#endif // DUK_SCENE_COMPONENT_REGISTRY_H

