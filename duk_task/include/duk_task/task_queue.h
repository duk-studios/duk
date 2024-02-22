/// 21/04/2023
/// job_queue.h

#ifndef DUK_TASK_TASK_QUEUE_H
#define DUK_TASK_TASK_QUEUE_H

#include <duk_macros/macros.h>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace duk::task {

namespace detail {
class Task {
public:
    virtual ~Task() = default;

    virtual void operator()() = 0;
};

template<typename F, std::enable_if_t<std::is_invocable_v<F&&>, int> = 0>
class TaskT : public Task {
public:
    explicit TaskT(F&& func) : m_task(std::forward<F>(func)) {
    }

    void operator()() override {
        m_task();
    }

private:
    F m_task;
};

template<typename F>
TaskT(F) -> TaskT<std::decay<F>>;

}// namespace detail

class TaskQueue {
public:
    TaskQueue();

    explicit TaskQueue(size_t threadCount);

    ~TaskQueue();

    template<typename F, typename... Args, std::enable_if_t<std::is_invocable_v<F&&, Args&&...>, int> = 0>
    std::future<std::invoke_result_t<F, Args...>> enqueue(F&& func, Args&&... args) {
        std::packaged_task<std::invoke_result_t<F, Args...>()> packagedTask(
                [_f = std::forward<F>(func),
                 _args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                    return std::apply(std::move(_f), std::move(_args));
                });

        auto future = packagedTask.get_future();

        auto task = std::unique_ptr<detail::Task>(new detail::TaskT(
                [t(std::move(packagedTask))]() mutable {
                    t();
                }));

        {
            std::unique_lock<std::mutex> lock(m_taskQueueMutex);
            m_taskQueue.emplace(std::move(task));
            m_awakeCondition.notify_one();
        }

        return future;
    }

    void start();

    void stop();

    void wait();

    DUK_NO_DISCARD bool owns_current_thread() const;

private:
    std::vector<std::thread> m_threads;
    std::vector<bool> m_workingThreads;
    std::queue<std::unique_ptr<detail::Task>> m_taskQueue;
    std::mutex m_taskQueueMutex;
    std::condition_variable m_awakeCondition;
    std::condition_variable m_idleCondition;
    bool m_running;
};

}// namespace duk::task

#endif// DUK_TASK_TASK_QUEUE_H
