/// 12/05/2023
/// command_scheduler.h

#ifndef DUK_RHI_COMMAND_SCHEDULER_H
#define DUK_RHI_COMMAND_SCHEDULER_H

#include <duk_rhi/command/command_queue.h>
#include <duk_rhi/pipeline/pipeline_flags.h>

#include <grapphs/adjacency_list.h>

namespace duk::rhi {

class CommandScheduler;

// opaque handle to connect commands
class ScheduledCommand {
public:
    explicit ScheduledCommand(CommandScheduler* owner, std::size_t index);
    // schedules "next" to be submitted after "this",
    // returns "next", so we can chain up multiple commands
    ScheduledCommand& then(ScheduledCommand& next, PipelineStage::Mask waitStages = PipelineStage::BOTTOM_OF_PIPE);

    ScheduledCommand& wait(ScheduledCommand& commandToWait, PipelineStage::Mask waitStages = PipelineStage::BOTTOM_OF_PIPE);

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
    void schedule_after(const ScheduledCommand& before, const ScheduledCommand& after, PipelineStage::Mask waitStages);

    // flushes all scheduled commands, taking into account described dependencies
    virtual void flush() = 0;


protected:

    virtual void schedule_after(std::size_t before, std::size_t after, PipelineStage::Mask waitStages) = 0;
};

}

#endif // DUK_RHI_COMMAND_SCHEDULER_H

