/// 21/04/2023
/// job_queue.cpp

#include <duk_task/task_queue.h>
#include <algorithm>

namespace duk::task {

TaskQueue::TaskQueue() : TaskQueue(1) {

}

TaskQueue::TaskQueue(size_t threadCount) :
    m_threads(threadCount),
    m_running(false) {

}

TaskQueue::~TaskQueue() {
    stop();
}

void TaskQueue::start() {
    if (m_running) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        m_running = true;
    }

    for (auto& thread : m_threads){
        thread = std::thread([this](){
            while (m_running){

                // waits until asked to quit or have some task available
                std::unique_lock<std::mutex> lock(m_taskQueueMutex);
                m_condition.wait(lock, [this]() -> bool {
                    return !m_running || !m_taskQueue.empty();
                });

                // if asked to quit, stop the loop now
                if (!m_running) {
                    return;
                }

                // gets next task
                auto task = std::move(m_taskQueue.front());
                m_taskQueue.pop();

                // releases the lock, so we can add more tasks to the queue from other threads
                lock.unlock();

                (*task)();
            }
        });
    }
}

void TaskQueue::stop() {

    {
        std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        m_running = false;
    }
    m_condition.notify_all();

    for (auto& thread : m_threads){
        if (thread.joinable()){
            thread.join();
        }
    }
}

bool TaskQueue::owns_current_thread() const {
    auto predicate = [](const std::thread& thread) {
        return thread.get_id() == std::this_thread::get_id();
    };
    return std::any_of(m_threads.begin(), m_threads.end(), predicate);
}

}

