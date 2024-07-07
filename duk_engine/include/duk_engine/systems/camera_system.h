//
// Created by Ricardo on 26/03/2024.
//

#ifndef DUK_ENGINE_CAMERA_SYSTEM_H
#define DUK_ENGINE_CAMERA_SYSTEM_H

#include <duk_system/system.h>

namespace duk::engine {

class CameraUpdateSystem : public duk::system::System {
public:
    void enter() override;

    void update() override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_CAMERA_SYSTEM_H
