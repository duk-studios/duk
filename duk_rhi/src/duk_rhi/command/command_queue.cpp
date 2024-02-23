/// 21/04/2023
/// command_queue.cpp

#include <duk_rhi/command/command_queue.h>

namespace duk::rhi {

CommandQueue::CommandQueue() {
    m_taskQueue.start();
}

CommandQueue::~CommandQueue() {
    m_taskQueue.stop();
}

}// namespace duk::rhi
