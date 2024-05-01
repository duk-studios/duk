//
// Created by Ricardo on 01/05/2024.
//

#include <duk_api/register_types.h>
#include <duk_api/api.h>

#include <duk_engine/register_types.h>
#include <duk_animation/register_types.h>

namespace duk::api {

void register_types() {
    duk::animation::register_types();
    duk::engine::register_types();
    duk_api_register_types();
}

}// namespace duk::api