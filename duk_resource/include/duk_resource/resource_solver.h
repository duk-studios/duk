/// 08/02/2024
/// resource_solver.h

#ifndef DUK_RESOURCE_RESOURCE_SOLVER_H
#define DUK_RESOURCE_RESOURCE_SOLVER_H

#include <duk_resource/pool.h>

namespace duk::resource {

class ResourceSolver {
public:

    template<typename T>
    void add_pool(PoolT<ResourceT<T>>* pool);

    // Solve a given resource that already has a set Id
    template<typename T>
    void solve(ResourceT<T>& resource);

    // Solve an object resources with template specialization (see solve_resources bellow)
    template<typename T>
    void solve(T& object);

private:

    template<typename T>
    using BaseResourceType = typename BaseResource<T>::Type;

    template<typename T>
    PoolT<ResourceT<BaseResourceType<T>>>* find_pool();

    template<typename T>
    static std::type_index type_index();


private:
    std::unordered_map<std::type_index, Pool*> m_pools;
};

template<typename T>
std::type_index ResourceSolver::type_index() {
    return typeid(BaseResourceType<T>);
}

// specialize method for types that need to solve their resources
template<typename T>
void solve_resources(ResourceSolver* solver, T& object) {}

template<typename T>
void ResourceSolver::add_pool(PoolT<ResourceT<T>>* pool) {
    const std::type_index type = type_index<T>();
    auto[it, inserted] = m_pools.emplace(type, pool);
    assert(inserted && "pool of same type already exists in ResourceSolver");
}

template<typename T>
void ResourceSolver::solve(ResourceT<T>& resource) {
    auto pool = find_pool<T>();
    if (!pool) {
        throw std::logic_error("could not solve resource: pool not found");
    }
    resource = pool->find(resource.id()).template as<T>();
}

template<typename T>
void ResourceSolver::solve(T& object) {
    solve_resources(this, object);
}

template<typename T>
PoolT<ResourceT<ResourceSolver::BaseResourceType<T>>>* ResourceSolver::find_pool() {
    const std::type_index type = type_index<T>();
    auto it = m_pools.find(type);
    if (it == m_pools.end()) {
        return nullptr;
    }
    return dynamic_cast<PoolT<ResourceT<BaseResourceType<T>>>*>(it->second);
}

}

#endif // DUK_RESOURCE_RESOURCE_SOLVER_H

