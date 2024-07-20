//
// Created by Ricardo on 14/07/2024.
//

#ifndef DUK_OBJECTS_OBJECT_SOLVER_H
#define DUK_OBJECTS_OBJECT_SOLVER_H

#include <duk_objects/objects.h>

namespace duk::objects {

class ObjectSolver {
public:
    explicit ObjectSolver(Objects& objects)
        : m_objects(objects)
        , m_runtimeIds(nullptr) {
    }

    ObjectSolver(Objects& objects, const std::unordered_map<Id, Id>& runtimeIds)
        : m_objects(objects)
        , m_runtimeIds(&runtimeIds) {
    }

    template<bool isConst>
    void solve(ObjectHandle<isConst>& object);

    template<typename T, bool isConst>
    void solve(ComponentHandle<T, isConst>& component);

    template<typename T>
    void solve(T& object);

private:
    Objects& m_objects;
    const std::unordered_map<Id, Id>* m_runtimeIds;
};

template<bool isConst>
void ObjectSolver::solve(ObjectHandle<isConst>& object) {
    auto id = object.id();
    if (m_runtimeIds) {
        auto it = m_runtimeIds->find(id);
        if (it != m_runtimeIds->end()) {
            id = it->second;
        }
    }
    object = m_objects.object(id);
}

template<typename T, bool isConst>
void ObjectSolver::solve(ComponentHandle<T, isConst>& component) {
    auto id = component.id();
    if (m_runtimeIds) {
        auto it = m_runtimeIds->find(id);
        if (it != m_runtimeIds->end()) {
            id = it->second;
        }
    }
    component = ComponentHandle<T, isConst>(m_objects.object(id));
}

template<typename T>
void ObjectSolver::solve(T& object) {
    duk::resource::solve_resources(this, object);
}

template<typename T>
void ComponentRegistry::ComponentEntryT<T>::solve(ObjectSolver* solver, ObjectHandle<false>& object) {
    auto component = ComponentHandle<T, false>(object);
    solver->solve(*component);
}

}// namespace duk::objects

#endif//DUK_OBJECTS_OBJECT_SOLVER_H
