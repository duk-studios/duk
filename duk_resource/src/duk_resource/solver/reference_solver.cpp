/// 08/02/2024
/// resource_solver.cpp

#include <duk_resource/solver/reference_solver.h>

namespace duk::resource {

ReferenceSolver::ReferenceSolver(Pools& pools)
    : m_pools(pools) {
}

}// namespace duk::resource