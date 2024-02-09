/// 09/02/2024
/// solver.h

#ifndef DUK_RESOURCE_SOLVER_H
#define DUK_RESOURCE_SOLVER_H

namespace duk::resource {

/*
 * Implement this method for types that have any resources references (or has members that reference resources):

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, YourType& obj) {
    solver->solve(obj.resource1);
    solver->solve(obj.resource2);
    solver->solve(obj.typeThatHasResourceMembers);
}

}
 */

template<typename Solver, typename T>
void solve_resources(Solver* solver, T& obj) {}

}

#endif // DUK_RESOURCE_SOLVER_H

