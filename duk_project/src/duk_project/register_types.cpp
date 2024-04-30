//
// Created by Ricardo on 27/04/2024.
//

#include <duk_project/register_types.h>

#include <duk_animation/register_types.h>

namespace duk::project {

void register_types() {
    duk::animation::register_types();
}

}// namespace duk::project
