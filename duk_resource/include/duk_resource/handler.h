/// 13/11/2023
/// handler.h

#ifndef DUK_RESOURCE_HANDLER_H
#define DUK_RESOURCE_HANDLER_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>
#include <duk_resource/resource.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <filesystem>

namespace duk::resource {

class ResourceHandler {
public:
    virtual ~ResourceHandler() = default;

    virtual const std::string& tag() const = 0;

    virtual bool accepts(const std::string& extension) const = 0;

    virtual Handle<void> load(Pools* pools, const Id& id, const std::filesystem::path& path) = 0;

    virtual void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) = 0;

    virtual void solve_references(const Handle<void>& handle, Pools* pools) = 0;
};

template<typename TPool>
class ResourceHandlerT : public ResourceHandler {
public:

    using Type = typename TPool::Type;

    explicit ResourceHandlerT(const char* tag);

    const std::string& tag() const final;

    Handle<void> load(Pools* pools, const Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) override;

    void solve_references(const Handle<void>& handle, Pools* pools) override;

protected:
    virtual Handle<Type> load(TPool* pool, const Id& id, const std::filesystem::path& path) = 0;

private:
    static TPool* find_pool(Pools* pools);

    std::string m_tag;
};

class ResourceRegistry {
public:
    static ResourceRegistry* instance();

    template<typename TResourceHandler>
    void add();

    ResourceHandler* find_handler(const std::string& tag);

    ResourceHandler* find_handler_for_extension(const std::string& extension);

private:
    std::unordered_map<std::string, std::unique_ptr<ResourceHandler>> m_handlers;
};

template<typename TResourceHandler>
void register_handler() {
    ResourceRegistry::instance()->add<TResourceHandler>();
}

template<typename TPool>
ResourceHandlerT<TPool>::ResourceHandlerT(const char* tag)
    : m_tag(tag) {
}

template<typename TPool>
const std::string& ResourceHandlerT<TPool>::tag() const {
    return m_tag;
}

template<typename TPool>
Handle<void> ResourceHandlerT<TPool>::load(Pools* pools, const Id& id, const std::filesystem::path& path) {
    auto pool = find_pool(pools);
    return load(pool, id, path);
}

template<typename TPool>
void ResourceHandlerT<TPool>::solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) {
    DependencySolver dependencySolver;
    dependencySolver.solve(*handle.as<Type>());
    dependencies = dependencySolver.dependencies();
}

template<typename TPool>
void ResourceHandlerT<TPool>::solve_references(const Handle<void>& handle, Pools* pools) {
    ReferenceSolver referenceSolver(pools);
    referenceSolver.solve(*handle.as<Type>());
}

template<typename TPool>
TPool* ResourceHandlerT<TPool>::find_pool(Pools* pools) {
    auto pool = pools->get<TPool>();
    if (!pool) {
        throw std::logic_error("Failed to find valid pool for ResourceHandlerT");
    }
    return pool;
}

template<typename TResourceHandler>
void ResourceRegistry::add() {
    static bool alreadyAdded = false;
    if (alreadyAdded) {
        return;
    }
    auto handler = std::make_unique<TResourceHandler>();
    m_handlers.emplace(handler->tag(), std::move(handler));
    alreadyAdded = true;
}

}// namespace duk::resource

#endif// DUK_RESOURCE_HANDLER_H
