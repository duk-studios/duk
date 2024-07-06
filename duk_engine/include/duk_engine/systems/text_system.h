//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_RENDERER_TEXT_SYSTEM_H
#define DUK_RENDERER_TEXT_SYSTEM_H

#include <duk_system/system.h>

namespace duk::engine {

class TextUpdateSystem : public duk::system::System {
public:
    void enter(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void update(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void exit(duk::objects::Objects& objects, duk::tools::Globals& globals) override;
};

}// namespace duk::engine

#endif//DUK_RENDERER_TEXT_SYSTEM_H
