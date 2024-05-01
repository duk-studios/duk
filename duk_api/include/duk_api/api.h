//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_API_H
#define DUK_API_H

namespace duk::engine {

class Engine;

}

extern "C" {

void duk_api_register_types();

void duk_api_run_enter(duk::engine::Engine& engine);

void duk_api_run_exit(duk::engine::Engine& engine);
}

#endif//DUK_API_H
