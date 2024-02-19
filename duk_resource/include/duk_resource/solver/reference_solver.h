/// 08/02/2024
/// resource_solver.h

#ifndef DUK_RESOURCE_REFERENCE_SOLVER_H
#define DUK_RESOURCE_REFERENCE_SOLVER_H

#include <duk_resource/pool.h>
#include <duk_resource/solver/solver.h>

namespace duk::resource {

class ReferenceSolver {
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
    PoolT<ResourceT<T>>* find_pool();

    template<typename T>
    static std::type_index type_index();


private:
    std::unordered_map<std::type_index, Pool*> m_pools;
};

template<typename T>
std::type_index ReferenceSolver::type_index() {
    return typeid(T);
}

template<typename T>
void ReferenceSolver::add_pool(PoolT<ResourceT<T>>* pool) {
    const std::type_index type = type_index<T>();
    auto[it, inserted] = m_pools.emplace(type, pool);
    assert(inserted && "pool of same type already exists in ReferenceSolver");
}

template<typename T>
void ReferenceSolver::solve(ResourceT<T>& resource) {
    auto pool = find_pool<T>();
    if (!pool) {
        throw std::logic_error("could not solve resource: pool not found");
    }
    resource = pool->find(resource.id()).template as<T>();
}

template<typename T>
void ReferenceSolver::solve(T& object) {
    solve_resources(this, object);
}

template<typename T>
PoolT<ResourceT<T>>* ReferenceSolver::find_pool() {
    const std::type_index type = type_index<T>();
    auto it = m_pools.find(type);
    if (it == m_pools.end()) {
        return nullptr;
    }
    return dynamic_cast<PoolT<ResourceT<T>>*>(it->second);
}

}

#endif // DUK_RESOURCE_REFERENCE_SOLVER_H

