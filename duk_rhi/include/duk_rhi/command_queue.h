/// 21/04/2023
/// command_queue.h

#ifndef DUK_RHI_COMMAND_QUEUE_H
#define DUK_RHI_COMMAND_QUEUE_H

#include <../command_context.h>

#include <duk_task/task_queue.h>

namespace duk::rhi {

class CommandQueue final {
public:
    struct Type {
        enum Bits : uint32_t {
            GRAPHICS = 1 << 0,
            COMPUTE = 1 << 1,
            TRANSFER = 1 << 2
        };

        static constexpr uint32_t kCount = 3;
        using Mask = uint32_t;
    };

public:
    CommandQueue(std::unique_ptr<CommandContext> context);

    ~CommandQueue();

    /// Returns the shared CommandContext for the device this queue belongs to.
    DUK_NO_DISCARD CommandContext* context() const;

    /// Submits a task to this queue. The task will be executed on the queue's thread, and
    /// the associated CommandContext will be passed as a parameter
    template<typename F, std::enable_if_t<std::is_void_v<std::invoke_result_t<F, CommandContext*>>, int> = 0>
    auto submit(F&& func) {
        return m_taskQueue.enqueue([this, taskFunc = std::forward<F>(func)]() {
            taskFunc(context());
        });
    }

protected:
    duk::task::TaskQueue m_taskQueue;
    std::unique_ptr<CommandContext> m_context;
};

}// namespace duk::rhi

#endif// DUK_RHI_COMMAND_QUEUE_H
