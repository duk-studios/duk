/// 13/11/2023
/// handler.h

#ifndef DUK_RESOURCE_HANDLER_H
#define DUK_RESOURCE_HANDLER_H

#include <duk_resource/pool.h>
#include <duk_resource/resource.h>
#include <duk_resource/file.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <filesystem>

namespace duk::resource {

class ResourceHandler {
public:
    virtual ~ResourceHandler() = default;

    virtual const std::string& tag() const = 0;

    virtual void load(Pools* pools, const Id& id, const std::filesystem::path& path) = 0;

    virtual void solve_dependencies(Pools* pools, const Id& id, DependencySolver& dependencySolver) = 0;

    virtual void solve_references(Pools* pools, const Id& id, ReferenceSolver& referenceSolver) = 0;
};

template<typename TPool>
class ResourceHandlerT : public ResourceHandler {
public:

    explicit ResourceHandlerT(const char* tag);

    const std::string& tag() const final;

    void load(Pools* pools, const Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(Pools* pools, const Id& id, DependencySolver& dependencySolver) override;

    void solve_references(Pools* pools, const Id& id, ReferenceSolver& referenceSolver) override;

protected:

    virtual void load(TPool* pool, const Id& id, const std::filesystem::path& path) = 0;

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
void ResourceHandlerT<TPool>::load(Pools* pools, const Id& id, const std::filesystem::path& path) {
    auto pool = find_pool(pools);
    load(pool, id, path);
}

template<typename TPool>
void ResourceHandlerT<TPool>::solve_dependencies(Pools* pools, const Id& id, DependencySolver& dependencySolver) {
    auto pool = find_pool(pools);
    auto resource = pool->find(id);
    if (!resource) {
        throw std::out_of_range(fmt::format("Resource ({}) not found in a pool", id.value()));
    }
    dependencySolver.solve(*resource);
}

template<typename TPool>
void ResourceHandlerT<TPool>::solve_references(Pools* pools, const Id& id, ReferenceSolver& referenceSolver) {
    auto pool = find_pool(pools);
    auto resource = pool->find(id);
    if (!resource) {
        throw std::out_of_range(fmt::format("Resource ({}) not found in a pool", id.value()));
    }
    referenceSolver.solve(*resource);
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
