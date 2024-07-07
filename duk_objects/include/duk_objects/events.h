//
// Created by Ricardo on 02/07/2024.
//

#ifndef DUK_OBJECTS_EVENTS_H
#define DUK_OBJECTS_EVENTS_H

#include <duk_objects/objects.h>

namespace duk::objects {

struct ObjectEnterEvent {};

template<typename C>
using ObjectEnterEventT = ComponentEvent<C, ObjectEnterEvent>;

struct ObjectExitEvent {};

template<typename C>
using ObjectExitEventT = ComponentEvent<C, ObjectExitEvent>;

struct ComponentEnterEvent {};

template<typename C>
using ComponentEnterEventT = ComponentEvent<C, ComponentEnterEvent>;

struct ComponentExitEvent {};

template<typename C>
using ComponentExitEventT = ComponentEvent<C, ComponentExitEvent>;

}// namespace duk::objects

#endif//DUK_OBJECTS_EVENTS_H
