/// 21/04/2023
/// command_queue.cpp

#include <duk_renderer/command_queue.h>

namespace duk::renderer {


CommandQueue::CommandQueue() {
    m_taskQueue.start();
}

CommandQueue::~CommandQueue() {
    m_taskQueue.stop();
}

}
