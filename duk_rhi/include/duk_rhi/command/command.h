/// 13/05/2023
/// command.h

#ifndef DUK_RHI_COMMAND_H
#define DUK_RHI_COMMAND_H

#include <duk_macros/macros.h>

#include <future>
#include <any>

namespace duk::rhi {

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
    DUK_NO_DISCARD T* submitter() {
        auto derived = dynamic_cast<T*>(submitter_ptr());
        if (!derived) {
            throw std::logic_error("invalid submitter_ptr type");
        }
        return derived;
    }

protected:
    DUK_NO_DISCARD virtual Submitter* submitter_ptr() = 0;
};

using FutureCommand = std::future<Command*>;

}

#endif // DUK_RHI_COMMAND_H

