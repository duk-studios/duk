//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_OBJECTS_OBJECTS_POOL_H
#define DUK_OBJECTS_OBJECTS_POOL_H

#include <duk_objects/objects.h>

#include <duk_resource/pool.h>

namespace duk::objects {

class ObjectsPool : public duk::resource::PoolT<ObjectsResource> {};

}// namespace duk::objects

#endif//DUK_OBJECTS_OBJECTS_POOL_H
