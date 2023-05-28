/// 12/05/2023
/// command_scheduler.h

#ifndef DUK_RENDERER_COMMAND_SCHEDULER_H
#define DUK_RENDERER_COMMAND_SCHEDULER_H

#include <duk_renderer/command/command_queue.h>

#include <grapphs/adjacency_list.h>

namespace duk::renderer {

class CommandScheduler;

// opaque handle to connect commands
class ScheduledCommand {
public:
    explicit ScheduledCommand(CommandScheduler* owner, std::size_t index);
    // schedules "next" to be submitted after "this",
    // returns "next", so we can chain up multiple commands
    ScheduledCommand& then(ScheduledCommand& next);

private:
    friend class CommandScheduler;
    CommandScheduler* m_owner;
    std::size_t m_index;
};

class CommandScheduler {
public:
    /// begins a new schedule, may wait for the completion of the last schedule
    virtual void begin() = 0;

    /// schedules a command for submission
    virtual ScheduledCommand schedule(Command* command) = 0;

    /// schedules a command that might not be ready yet, will wait for it just before submitting
    virtual ScheduledCommand schedule(FutureCommand&& futureCommand) = 0;

    /// schedules a command to be executed AFTER given scheduled command
    void schedule_after(const ScheduledCommand& before, const ScheduledCommand& after);

    // flushes all scheduled commands, taking into account described dependencies
    virtual void flush() = 0;


protected:

    virtual void schedule_after(std::size_t before, std::size_t after) = 0;
};

}

#endif // DUK_RENDERER_COMMAND_SCHEDULER_H

