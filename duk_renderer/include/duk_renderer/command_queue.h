/// 21/04/2023
/// command_queue.h

#ifndef DUK_RENDERER_COMMAND_QUEUE_H
#define DUK_RENDERER_COMMAND_QUEUE_H

#include <duk_renderer/command_interface.h>

#include <duk_task/task_queue.h>

namespace duk::renderer {

enum CommandQueueType {
    QUEUE_GRAPHICS = 0,
    QUEUE_COMPUTE = 1,
    QUEUE_COUNT
};

struct CommandQueueCreateInfo {
    CommandQueueType type;
};

class CommandQueue {
public:

    CommandQueue();

    virtual ~CommandQueue();

    template <typename F, typename... Args>
    auto enqueue(F&& func, Args&&... args) {
        return m_taskQueue.template enqueue(func, m_commandInterface, std::forward<Args>(args)...);
    }

private:
    CommandInterface* m_commandInterface;
    duk::task::TaskQueue m_taskQueue;
};

}

#endif // DUK_RENDERER_COMMAND_QUEUE_H

