/// 09/02/2024
/// dependency_solver.h

#ifndef DUK_RESOURCE_DEPENDENCY_SOLVER_H
#define DUK_RESOURCE_DEPENDENCY_SOLVER_H

#include <duk_resource/resource.h>
#include <duk_resource/solver/solver.h>
#include <set>

namespace duk::resource {

class DependencySolver {
public:

    template<typename T>
    void solve(ResourceT<T>& resource);

    template<typename T>
    void solve(T& object);

    void clear();

    size_t size() const;

    DUK_NO_DISCARD const std::set<Id>& dependencies() const;

private:
    std::set<Id> m_dependencies;
};

template<typename T>
void DependencySolver::solve(ResourceT<T>& resource) {
    const bool alreadyInserted = m_dependencies.find(resource.id()) != m_dependencies.end();
    m_dependencies.insert(resource.id());
    if (!alreadyInserted) {
        solve_resources(this, *resource); // get dependencies of this resource
    }
}

template<typename T>
void DependencySolver::solve(T& object) {
    solve_resources(this, object);
}

}

#endif // DUK_RESOURCE_DEPENDENCY_SOLVER_H
