/// 21/04/2023
/// command_queue.h

#ifndef DUK_RENDERER_COMMAND_QUEUE_H
#define DUK_RENDERER_COMMAND_QUEUE_H

#include <duk_renderer/command_buffer.h>

#include <duk_task/task_queue.h>

namespace duk::renderer {

enum CommandQueueType {
    QUEUE_GRAPHICS = 0,
    QUEUE_COMPUTE = 1,
    QUEUE_COUNT
};

class CommandQueue {
public:

    CommandQueue();

    virtual ~CommandQueue();

    template<typename F, typename ...Args, std::enable_if_t<std::is_void_v<std::invoke_result_t<F, CommandBuffer*, Args&&...>>, int> = 0>
    FutureCommand enqueue(F&& func, Args&&... args) {
        return m_taskQueue.template enqueue([
                this,
                taskFunc = std::forward<F>(func),
                &args...
                ]() -> Command* {
            auto commandBuffer = next_command_buffer();
            taskFunc(commandBuffer, std::forward<Args>(args)...);
            return static_cast<Command*>(commandBuffer);
        });
    }

protected:

    virtual CommandBuffer* next_command_buffer() = 0;

private:
    duk::task::TaskQueue m_taskQueue;
};

}

#endif // DUK_RENDERER_COMMAND_QUEUE_H

