//
// Created by Ricardo on 16/08/2024.
//

#ifndef DUK_SYSTEM_SYSTEM_SOLVER_H
#define DUK_SYSTEM_SYSTEM_SOLVER_H

#include <duk_system/system.h>

namespace duk::system {

// this is a TERRIBLE hack and needs to be fixed
// long story short: dealing with template defifinitions/specializations causes lots of issues
// we need to completely rethink our registry implementation
template<typename T>
void SystemRegistry::SystemEntryT<T>::solve(duk::objects::ObjectSolver* solver, Systems& systems) {
    solver->solve(*systems.get<T>());
}

}// namespace duk::system

#endif//DUK_SYSTEM_SYSTEM_SOLVER_H
