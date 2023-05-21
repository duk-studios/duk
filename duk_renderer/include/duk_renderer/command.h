/// 13/05/2023
/// command.h

#ifndef DUK_RENDERER_COMMAND_H
#define DUK_RENDERER_COMMAND_H

#include <duk_macros/macros.h>

#include <future>
#include <any>

namespace duk::renderer {

/// used by CommandScheduler to execute the submission
/// this is necessary so we don't expose unnecessary methods to the user
class Submitter {
public:
    virtual ~Submitter() = default;
};

/// Represents any kind of command that is submitted to the gpu
/// eg. CommandBuffer, image acquire command, present command
class Command {
public:
    virtual ~Command() = default;

    template<typename T>
    DUK_NO_DISCARD const T* submitter() const {
        auto derived = dynamic_cast<const T*>(submitter_ptr());
        if (!derived) {
            throw std::logic_error("invalid submitter_ptr type");
        }
        return derived;
    }

protected:
    DUK_NO_DISCARD virtual const Submitter* submitter_ptr() const = 0;
};

using FutureCommand = std::future<Command*>;

}

#endif // DUK_RENDERER_COMMAND_H

