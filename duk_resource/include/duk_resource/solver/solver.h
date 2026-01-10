/// 09/02/2024
/// solver.h

#ifndef DUK_RESOURCE_SOLVER_H
#define DUK_RESOURCE_SOLVER_H

#include <duk_type/describe_class.h>
#include <duk_type/describe_container.h>

namespace duk::resource {

/*
 * Types that specialize duk::type::Type<T> can be solved for resources using this function.
 * It will recursively visit all members and elements of the object and call the solver's solve method on them.
 * Some primitives (such as smart pointers) have specializations to dereference and solve the underlying object.
 */

template<typename Solver, typename T>
void solve_resources(Solver* solver, T& obj);

template<typename Solver, typename T>
struct ObjectResourceSolver {
    static void solve(Solver* solver, T& obj);
};

template<typename Solver, typename T>
struct ContainerResourceSolver {
    static void solve(Solver* solver, T& container);
};

template<typename Solver, typename T>
struct PrimitiveResourceSolver {
    static void solve(Solver* solver, T& obj);
};

template<typename Solver, typename T>
struct PrimitiveResourceSolver<Solver, std::shared_ptr<T>> {
    static void solve(Solver* solver, std::shared_ptr<T>& obj);
};

template<typename Solver, typename T>
struct PrimitiveResourceSolver<Solver, std::unique_ptr<T>> {
    static void solve(Solver* solver, std::unique_ptr<T>& obj);
};

template<typename Solver, typename T>
void solve_resources(Solver* solver, T& obj) {
    if constexpr (duk::type::is_class<T>()) {
        ObjectResourceSolver<Solver, T>::solve(solver, obj);
    }
    else if constexpr (duk::type::is_container<T>()) {
        ContainerResourceSolver<Solver, T>::solve(solver, obj);
    }
    else {
        PrimitiveResourceSolver<Solver, T>::solve(solver, obj);
    }
}

template<typename Solver, typename T>
void ObjectResourceSolver<Solver, T>::solve(Solver* solver, T& obj) {
    constexpr auto description = duk::type::describe<T>();
    description.visit_members([&](const auto& member) {
        solver->solve(member.value());
    }, obj);
}

template<typename Solver, typename T>
void ContainerResourceSolver<Solver, T>::solve(Solver* solver, T& container) {
    constexpr auto description = duk::type::describe<T>();
    description.visit_elements([&](auto& element) {
        solver->solve(element);
    }, container);
}

template<typename Solver, typename T>
void PrimitiveResourceSolver<Solver, T>::solve(Solver* solver, T& obj) {
    // no-op for primitive types
}

template<typename Solver, typename T>
void PrimitiveResourceSolver<Solver, std::shared_ptr<T>>::solve(Solver* solver, std::shared_ptr<T>& obj) {
    if (obj) {
        solve_resources(solver, *obj);
    }
}

template<typename Solver, typename T>
void PrimitiveResourceSolver<Solver, std::unique_ptr<T>>::solve(Solver* solver, std::unique_ptr<T>& obj) {
    if (obj) {
        solve_resources(solver, *obj);
    }
}

}// namespace duk::resource

#endif// DUK_RESOURCE_SOLVER_H
