/// 13/05/2023
/// command.h

#ifndef DUK_RENDERER_COMMAND_H
#define DUK_RENDERER_COMMAND_H

#include <future>

namespace duk::renderer {

/// Represents any kind of command that is run on the gpu
/// eg. CommandBuffer, image acquire command, present command
class Command {
public:
    virtual ~Command() = default;
};

using FutureCommand = std::future<Command*>;

}

#endif // DUK_RENDERER_COMMAND_H

