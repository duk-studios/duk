/// 21/04/2023
/// command_queue.cpp

#include <duk_rhi/command_queue.h>
#include <duk_rhi/command_context.h>

namespace duk::rhi {

CommandQueue::CommandQueue(std::unique_ptr<CommandContext> context)
    : m_context(std::move(context)) {
    m_taskQueue.start();
}

CommandQueue::~CommandQueue() {
    m_taskQueue.stop();
}

CommandContext* CommandQueue::context() const {
    return m_context.get();
}

}// namespace duk::rhi
