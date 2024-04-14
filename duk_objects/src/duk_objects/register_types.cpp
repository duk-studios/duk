//
// Created by Ricardo on 13/04/2024.
//
#include <duk_objects/objects_handler.h>
#include <duk_objects/register_types.h>

namespace duk::objects {

void register_types() {
    duk::resource::register_handler<ObjectsHandler>();
}

}// namespace duk::objects
