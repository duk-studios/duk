/// 08/02/2024
/// resource_solver.h

#ifndef DUK_RESOURCE_REFERENCE_SOLVER_H
#define DUK_RESOURCE_REFERENCE_SOLVER_H

#include <duk_resource/pool.h>
#include <duk_resource/solver/solver.h>

namespace duk::resource {

class ReferenceSolver {
public:

    explicit ReferenceSolver(Pools* pools);

    // Solve a given resource that already has a set Id
    template<typename T>
    void solve(ResourceT<T>& resource);

    // Solve an object resources with template specialization (see solve_resources bellow)
    template<typename T>
    void solve(T& object);

private:

    template<typename T>
    PoolT<ResourceT<T>>* find_pool();

private:
    Pools* m_pools;
};


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
    return m_pools->get<PoolT<ResourceT<T>>>();
}

}

#endif // DUK_RESOURCE_REFERENCE_SOLVER_H

