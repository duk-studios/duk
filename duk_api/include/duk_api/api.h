//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_API_H
#define DUK_API_H

namespace duk::tools {

class Globals;

}

extern "C" {

void duk_api_register_types();

void duk_api_run_enter(duk::tools::Globals& globals);

void duk_api_run_exit(duk::tools::Globals& globals);
}

#endif//DUK_API_H
