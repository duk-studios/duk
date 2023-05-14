/// 12/05/2023
/// vulkan_command_scheduler.cpp

#include <duk_renderer/vulkan/vulkan_command_scheduler.h>
#include <duk_renderer/vulkan/vulkan_command_queue.h>
#include <duk_renderer/vulkan/vulkan_command_buffer.h>

#include <grapphs/algorithms/bfs_traversal.h>

namespace duk::renderer {

VulkanCommandScheduler::VulkanCommandScheduler(const VulkanCommandSchedulerCreateInfo& commandSchedulerCreateInfo) :
    m_device(commandSchedulerCreateInfo.device),
    m_currentFramePtr(commandSchedulerCreateInfo.currentFramePtr) {

    FrameCreateInfo frameCreateInfo = {};
    frameCreateInfo.device = m_device;

    m_frames.reserve(commandSchedulerCreateInfo.frameCount);
    for (auto i = 0; i < commandSchedulerCreateInfo.frameCount; i++) {
        m_frames.emplace_back(frameCreateInfo);
    }
}

void VulkanCommandScheduler::begin() {
    auto& frame = m_frames[*m_currentFramePtr];
    // clear frame to begin a new one
    frame.clear();
}

ScheduledCommand VulkanCommandScheduler::schedule(Command* command) {
    // create a promise and set its value immediately
    std::promise<Command*> promise;
    promise.set_value(command);
    return schedule(promise.get_future());
}

ScheduledCommand VulkanCommandScheduler::schedule(FutureCommand&& futureCommand) {
    auto& frame = m_frames[*m_currentFramePtr];
    auto index = frame.schedule_command(std::move(futureCommand));

    return ScheduledCommand(this, index);
}

void VulkanCommandScheduler::schedule_after(std::size_t before, std::size_t after) {
    auto& frame = m_frames[*m_currentFramePtr];
    frame.schedule_after(before, after);
}

void VulkanCommandScheduler::flush() {
    auto& frame = m_frames[*m_currentFramePtr];
    frame.flush();
}

VulkanCommandScheduler::CommandNode::CommandNode(FutureCommand&& futureCommand, VulkanSemaphorePool* semaphorePool) noexcept :
    m_futureCommand(std::move(futureCommand)),
    m_semaphorePool(semaphorePool),
    m_semaphore(m_semaphorePool->allocate()){

}

VulkanCommandScheduler::CommandNode::~CommandNode() {
    m_semaphorePool->free(m_semaphore);
}

void VulkanCommandScheduler::CommandNode::submit(VkFence* fence) {
    VulkanWaitDependency waitDependency = {};
    waitDependency.semaphores = m_waitSemaphores.data();
    waitDependency.semaphoreCount = m_waitSemaphores.size();
    waitDependency.stages = m_waitStages.data();

    VulkanCommandParams params = {};
    params.waitDependency = &waitDependency;
    params.signalSemaphore = &m_semaphore;
    params.fence = fence;

    // does this even work?
    auto command = reinterpret_cast<VulkanCommand*>(m_futureCommand.get());
    command->submit(params);
}

VkSemaphore& VulkanCommandScheduler::CommandNode::semaphore() {
    return m_semaphore;
}

void VulkanCommandScheduler::CommandNode::wait(const VkSemaphore& semaphore, VkPipelineStageFlags stage) {
    m_waitSemaphores.push_back(semaphore);
    m_waitStages.push_back(stage);
}

VulkanCommandScheduler::Frame::Frame(const VulkanCommandScheduler::FrameCreateInfo& frameCreateInfo) :
    m_device(frameCreateInfo.device) {
    VulkanFencePoolCreateInfo fencePoolCreateInfo = {};
    fencePoolCreateInfo.device = m_device;
    fencePoolCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_fencePool = std::make_unique<VulkanFencePool>(fencePoolCreateInfo);

    VulkanSemaphorePoolCreateInfo semaphorePoolCreateInfo = {};
    semaphorePoolCreateInfo.device = m_device;

    m_semaphorePool = std::make_unique<VulkanSemaphorePool>(semaphorePoolCreateInfo);
}

VulkanCommandScheduler::Frame::Frame(VulkanCommandScheduler::Frame&& other) noexcept = default;

VulkanCommandScheduler::Frame::~Frame() {
    clear();
}

void VulkanCommandScheduler::Frame::wait() {
    if (!m_fences.empty()) {
        vkWaitForFences(m_device, m_fences.size(), m_fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    }
}

void VulkanCommandScheduler::Frame::clear() {
    wait();
    m_independentNodes.clear();
    m_scheduledCommands.clear();
    m_waitDependencies.clear();
    m_fencePool->free(m_fences);
    m_fences.clear();
}

std::size_t VulkanCommandScheduler::Frame::schedule_command(FutureCommand&& futureCommand) {
    auto index = m_scheduledCommands.push(std::make_shared<CommandNode>(std::move(futureCommand), m_semaphorePool.get()));

    // we assume that every node is independent, until it is not
    m_independentNodes.insert(index);

    return index;
}

void VulkanCommandScheduler::Frame::schedule_after(std::size_t before, std::size_t after) {
    m_scheduledCommands.connect(before, after, CommandConnection{});
    m_independentNodes.erase(after);
}

void VulkanCommandScheduler::Frame::flush() {

    auto perNode = [this](std::size_t index) {
        auto commandNode = m_scheduledCommands.vertex(index)->get();

        auto fence = m_fences.emplace_back(m_fencePool->allocate());

        commandNode->submit(&fence);
    };

    auto perEdge = [this](std::size_t fromIndex, std::size_t toIndex) {
        auto from = m_scheduledCommands.vertex(fromIndex)->get();
        auto to = m_scheduledCommands.vertex(toIndex)->get();

        to->wait(from->semaphore(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    };

    gpp::breadth_first_traverse(m_scheduledCommands, m_independentNodes, perNode, perEdge);
}

}