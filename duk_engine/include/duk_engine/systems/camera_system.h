//
// Created by Ricardo on 26/03/2024.
//

#ifndef DUK_ENGINE_CAMERA_SYSTEM_H
#define DUK_ENGINE_CAMERA_SYSTEM_H

#include <duk_system/system.h>

namespace duk::engine {

class CameraUpdateSystem : public duk::system::System {
public:
    void enter(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void update(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void exit(duk::objects::Objects& objects, duk::tools::Globals& globals) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_CAMERA_SYSTEM_H
