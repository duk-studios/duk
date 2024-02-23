/// 12/05/2023
/// command_scheduler.cpp

#include <duk_rhi/command/command_scheduler.h>

namespace duk::rhi {

ScheduledCommand::ScheduledCommand(CommandScheduler* owner, std::size_t index)
    : m_owner(owner)
    , m_index(index) {
}

ScheduledCommand& ScheduledCommand::then(ScheduledCommand& next, PipelineStage::Mask waitStages) {
    m_owner->schedule_after(*this, next, waitStages);
    return next;
}

ScheduledCommand& ScheduledCommand::wait(ScheduledCommand& commandToWait, PipelineStage::Mask waitStages) {
    m_owner->schedule_after(commandToWait, *this, waitStages);
    return *this;
}

void CommandScheduler::schedule_after(const ScheduledCommand& before, const ScheduledCommand& after, PipelineStage::Mask waitStages) {
    schedule_after(before.m_index, after.m_index, waitStages);
}

}// namespace duk::rhi
