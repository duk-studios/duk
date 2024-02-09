/// 09/02/2024
/// dependency_solver.cpp

#include <duk_resource/solver/dependency_solver.h>

namespace duk::resource {

void DependencySolver::clear() {
    m_dependencies.clear();
}

size_t DependencySolver::size() const {
    return m_dependencies.size();
}

const std::set<Id>& DependencySolver::dependencies() const {
    return m_dependencies;
}

}
