/// 21/04/2023
/// command_queue.h

#ifndef DUK_RHI_COMMAND_QUEUE_H
#define DUK_RHI_COMMAND_QUEUE_H

#include <duk_rhi/command/command_buffer.h>

#include <duk_task/task_queue.h>

namespace duk::rhi {

class CommandQueue {
public:
    struct Type {
        enum Bits : uint32_t {
            GRAPHICS = 1 << 0,
            COMPUTE = 1 << 1,
            PRESENT = 1 << 2
        };

        static constexpr uint32_t kCount = 3;
        using Mask = uint32_t;
    };

public:
    CommandQueue();

    virtual ~CommandQueue();

    template<typename F, typename... Args, std::enable_if_t<std::is_void_v<std::invoke_result_t<F, CommandBuffer*, Args&&...>>, int> = 0>
    FutureCommand submit(F&& func, Args&&... args) {
        return m_taskQueue.enqueue([this,
                                    taskFunc = std::forward<F>(func),
                                    &args...]() -> Command* {
            auto commandBuffer = next_command_buffer();
            m_currentCommandBuffer = commandBuffer;
            taskFunc(commandBuffer, std::forward<Args>(args)...);
            m_currentCommandBuffer = nullptr;
            return static_cast<Command*>(commandBuffer);
        });
    }

protected:
    virtual CommandBuffer* next_command_buffer() = 0;

protected:
    duk::task::TaskQueue m_taskQueue;

    // may be used by derived classes during the enqueue callback execution
    CommandBuffer* m_currentCommandBuffer;
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_QUEUE_H
