/// 12/05/2023
/// command_scheduler.cpp

#include <duk_renderer/command_scheduler.h>


namespace duk::renderer {

ScheduledCommand::ScheduledCommand(CommandScheduler* owner, std::size_t index) :
        m_owner(owner),
        m_index(index) {

}

ScheduledCommand& ScheduledCommand::then(ScheduledCommand& next) {
    m_owner->schedule_after(*this, next);
    return next;
}

void CommandScheduler::schedule_after(const ScheduledCommand& before, const ScheduledCommand& after) {
    schedule_after(before.m_index, after.m_index);
}

}
