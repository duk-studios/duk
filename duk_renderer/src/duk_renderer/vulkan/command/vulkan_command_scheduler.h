/// 12/05/2023
/// vulkan_command_scheduler.h

#ifndef DUK_RENDERER_VULKAN_COMMAND_SCHEDULER_H
#define DUK_RENDERER_VULKAN_COMMAND_SCHEDULER_H

#include <duk_renderer/command/command_scheduler.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/command/vulkan_command.h>
#include <duk_renderer/vulkan/vulkan_handle_pool.h>

#include <grapphs/adjacency_list.h>

namespace duk::renderer {

class VulkanCommandQueue;

struct VulkanCommandSchedulerCreateInfo {
    uint32_t* currentFramePtr;
    uint32_t frameCount;
    VkDevice device;
};

class VulkanCommandScheduler : public CommandScheduler {
public:
    VulkanCommandScheduler(const VulkanCommandSchedulerCreateInfo& commandSchedulerCreateInfo);

    void begin() override;

    ScheduledCommand schedule(Command* command) override;

    ScheduledCommand schedule(FutureCommand&& futureCommand) override;

    void flush() override;

    void schedule_after(std::size_t before, std::size_t after, PipelineStage::Mask waitStages) override;

private:

    class CommandNode {
    public:
        explicit CommandNode(FutureCommand&& futureCommand) noexcept;

        ~CommandNode();

        void submit(VkFence& fence, uint32_t waitSemaphoreCount, VkSemaphore* waitSemaphores, VkPipelineStageFlags* waitStages);

        Command* command();

    private:
        FutureCommand m_futureCommand;
        Command* m_command;
    };

    struct CommandConnection {
        PipelineStage::Mask waitStages;
    };

    struct FrameCreateInfo {
        VkDevice device;
    };

    class Frame {
    public:

        explicit Frame(const FrameCreateInfo& frameCreateInfo);

        Frame(Frame&& other) noexcept;

        ~Frame();

        // waits for all fences to be signaled, call this before starting a new frame
        void wait();

        // clear all resources, call this to start a new frame
        void clear();

        // submits all scheduled commands
        void flush();

        std::size_t schedule_command(FutureCommand&& commandNode);

        void schedule_after(std::size_t before, std::size_t after, PipelineStage::Mask waitStages);

    private:
        VkDevice m_device;

        // stores all dependencies for a given command node, this is filled during scheduled commands traverse
        std::unordered_map<CommandNode*, VulkanWaitDependency> m_waitDependencies;

        // use a shared_ptr, as the command batch can be a polymorphic type
        // it also needs to be copy assignable, so unique_ptr is no good here
        gpp::adjacency_list<std::shared_ptr<CommandNode>, CommandConnection> m_scheduledCommands;
        std::set<std::size_t> m_independentNodes;

        struct Dependency {
            std::size_t index;
            PipelineStage::Mask waitStages;

            bool operator<(const Dependency& rhs) const noexcept;

            bool operator==(const Dependency& rhs) const noexcept;
        };

        // maps a command node to a set of command nodes which its dependencies (e.g. a set of command nodes which we must wait for its semaphores)
        std::unordered_map<std::size_t, std::set<Dependency>> m_commandDependencies;

        // fences to check if this frame is being executed
        std::vector<VkFence> m_fences;
    };

private:
    VkDevice m_device;
    uint32_t* m_currentFramePtr;
    std::vector<Frame> m_frames;
};

}


#endif // DUK_RENDERER_VULKAN_COMMAND_SCHEDULER_H

