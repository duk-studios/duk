/// 12/05/2023
/// vulkan_command_scheduler.cpp

#include <duk_renderer/vulkan/command/vulkan_command_scheduler.h>
#include <duk_renderer/vulkan/command/vulkan_command_queue.h>
#include <duk_renderer/vulkan/command/vulkan_command_buffer.h>

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

VulkanCommandScheduler::CommandNode::CommandNode(FutureCommand&& futureCommand) noexcept :
    m_futureCommand(std::move(futureCommand)),
    m_command(nullptr){

}

VulkanCommandScheduler::CommandNode::~CommandNode() = default;

void VulkanCommandScheduler::CommandNode::submit(VkFence& fence, uint32_t waitSemaphoreCount, VkSemaphore* waitSemaphores, VkPipelineStageFlags* waitStages) {
    // not my brightest moment, but oh well
    // it is what it is
    m_command = m_futureCommand.get();
    const auto submitter = m_command->submitter<VulkanSubmitter>();

    VulkanWaitDependency waitDependency = {};
    waitDependency.semaphoreCount = waitSemaphoreCount;
    waitDependency.semaphores = waitSemaphores;
    waitDependency.stages = waitStages;

    if (submitter->signals_fence()) {
        fence = *submitter->fence();
    }

    submitter->submit(&waitDependency);
}

Command* VulkanCommandScheduler::CommandNode::command() {
    assert(m_command && "access to a command must happen after submission");
    return m_command;
}

VulkanCommandScheduler::Frame::Frame(const VulkanCommandScheduler::FrameCreateInfo& frameCreateInfo) :
    m_device(frameCreateInfo.device) {
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
    m_fences.clear();
}

std::size_t VulkanCommandScheduler::Frame::schedule_command(FutureCommand&& futureCommand) {
    auto index = m_scheduledCommands.push(std::make_shared<CommandNode>(std::move(futureCommand)));

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

        std::vector<VkSemaphore> semaphores;
        std::vector<VkPipelineStageFlags> stages;

        auto& dependencies = m_commandDependencies[index];
        for (auto dependency : dependencies) {
            auto& dependencyCommandNode = *m_scheduledCommands.vertex(dependency);
            auto dependencyCommand = dependencyCommandNode->command();
            auto dependencySubmitter = dependencyCommand->submitter<VulkanSubmitter>();

            if (dependencySubmitter->signals_semaphore()) {
                semaphores.push_back(*dependencySubmitter->semaphore());
                stages.push_back(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
            }
        }

        VkFence fence = VK_NULL_HANDLE;

        commandNode->submit(fence, semaphores.size(), semaphores.data(), stages.data());

        if (fence != VK_NULL_HANDLE) {
            m_fences.push_back(fence);
        }
    };

    auto perEdge = [this](std::size_t fromIndex, std::size_t toIndex) {
        m_commandDependencies[toIndex].insert(fromIndex);
    };

    gpp::breadth_first_traverse(m_scheduledCommands, m_independentNodes, perNode, perEdge);
}

}