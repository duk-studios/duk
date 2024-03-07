//
// Created by Ricardo on 07/03/2024.
//

#ifndef DUK_RENDERER_ENVIRONMENT_H
#define DUK_RENDERER_ENVIRONMENT_H

#include <duk_platform/window.h>
#include <duk_scene/systems.h>

namespace duk::renderer {

class Renderer;

class Environment : public duk::scene::Environment {
public:
    virtual duk::platform::Window* window() = 0;

    virtual Renderer* renderer() = 0;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_ENVIRONMENT_H
