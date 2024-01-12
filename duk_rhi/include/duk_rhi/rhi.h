//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RHI_RENDERER_H
#define DUK_RHI_RENDERER_H

#include <duk_rhi/renderer_error.h>
#include <duk_rhi/rhi_capabilities.h>
#include <duk_rhi/command/command_queue.h>
#include <duk_rhi/command/command_scheduler.h>
#include <duk_rhi/render_pass.h>
#include <duk_rhi/buffer.h>
#include <duk_rhi/image.h>
#include <duk_rhi/image_data_source.h>
#include <duk_rhi/descriptor_set.h>
#include <duk_rhi/frame_buffer.h>
#include <duk_rhi/pipeline/shader_data_source.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/pipeline/compute_pipeline.h>

#include <duk_log/logger.h>

#include <tl/expected.hpp>

#include <memory>
#include <optional>

namespace duk::platform {

class Window;

}

namespace duk::rhi {

enum class API {
    UNDEFINED,
    VULKAN,
    OPENGL45,
    DX12
};

class RHI;
using ExpectedRHI = tl::expected<std::shared_ptr<RHI>, RendererError>;
using ExpectedCommandQueue = tl::expected<std::shared_ptr<CommandQueue>, RendererError>;
using ExpectedShader = tl::expected<std::shared_ptr<Shader>, RendererError>;
using ExpectedGraphicsPipeline = tl::expected<std::shared_ptr<GraphicsPipeline>, RendererError>;
using ExpectedComputePipeline = tl::expected<std::shared_ptr<ComputePipeline>, RendererError>;
using ExpectedRenderPass = tl::expected<std::shared_ptr<RenderPass>, RendererError>;
using ExpectedFrameBuffer = tl::expected<std::shared_ptr<FrameBuffer>, RendererError>;
using ExpectedBuffer = tl::expected<std::shared_ptr<Buffer>, RendererError>;
using ExpectedImage = tl::expected<std::shared_ptr<Image>, RendererError>;
using ExpectedDescriptorSet = tl::expected<std::shared_ptr<DescriptorSet>, RendererError>;
using ExpectedCommandScheduler = tl::expected<std::shared_ptr<CommandScheduler>, RendererError>;

struct RHICreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    const char* applicationName;
    uint32_t applicationVersion;
    const char* engineName;
    uint32_t engineVersion;
    API api;
    size_t deviceIndex;
};

class RHI {
public:

    static ExpectedRHI create_rhi(const RHICreateInfo& rhiCreateInfo);

    /// destructor
    virtual ~RHI() = default;

    /// called to start a new frame
    virtual void prepare_frame() = 0;

    /// returns a command that acquires the next image for presentation
    DUK_NO_DISCARD virtual Command* acquire_image_command() = 0;

    /// returns a command that presents the contents of the present image on the screen
    DUK_NO_DISCARD virtual Command* present_command() = 0;

    /// returns the current present image
    DUK_NO_DISCARD virtual Image* present_image() = 0;

    /// returns an object that can be used to check for limits and capabilities of this rhi
    DUK_NO_DISCARD virtual RHICapabilities* capabilities() const = 0;

    struct CommandQueueCreateInfo {
        CommandQueue::Type::Bits type;
    };

    DUK_NO_DISCARD virtual ExpectedCommandQueue create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) = 0;

    DUK_NO_DISCARD virtual ExpectedCommandScheduler create_command_scheduler() = 0;

    struct ShaderCreateInfo {
        const ShaderDataSource* shaderDataSource;
    };

    DUK_NO_DISCARD virtual ExpectedShader create_shader(const ShaderCreateInfo& shaderCreateInfo) = 0;

    struct GraphicsPipelineCreateInfo {
        Shader* shader;
        RenderPass* renderPass;
        GraphicsPipeline::Viewport viewport;
        GraphicsPipeline::Scissor scissor;
        GraphicsPipeline::CullMode::Mask cullModeMask;
        GraphicsPipeline::Blend blend;
        GraphicsPipeline::Topology topology;
        GraphicsPipeline::FillMode fillMode;
        bool depthTesting;
    };

    DUK_NO_DISCARD virtual ExpectedGraphicsPipeline create_graphics_pipeline(const GraphicsPipelineCreateInfo& pipelineCreateInfo) = 0;

    struct ComputePipelineCreateInfo {
        Shader* shader;
    };

    DUK_NO_DISCARD virtual ExpectedComputePipeline create_compute_pipeline(const ComputePipelineCreateInfo& pipelineCreateInfo) = 0;

    struct RenderPassCreateInfo {
        AttachmentDescription* colorAttachments;
        uint32_t colorAttachmentCount;
        AttachmentDescription* depthAttachment;
    };

    DUK_NO_DISCARD virtual ExpectedRenderPass create_render_pass(const RenderPassCreateInfo& renderPassCreateInfo) = 0;

    struct BufferCreateInfo {
        Buffer::Type type;
        Buffer::UpdateFrequency updateFrequency;
        size_t elementCount;
        size_t elementSize;
        CommandQueue* commandQueue;
    };

    DUK_NO_DISCARD virtual ExpectedBuffer create_buffer(const BufferCreateInfo& bufferCreateInfo) = 0;

    struct ImageCreateInfo {
        const ImageDataSource* imageDataSource;
        Image::Layout initialLayout;
        Image::Usage usage;
        Image::UpdateFrequency updateFrequency;
        CommandQueue* commandQueue;
    };

    DUK_NO_DISCARD virtual ExpectedImage create_image(const ImageCreateInfo& imageCreateInfo) = 0;

    struct DescriptorSetCreateInfo {
        DescriptorSetDescription description;
    };

    DUK_NO_DISCARD virtual ExpectedDescriptorSet create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) = 0;

    struct FrameBufferCreateInfo {
        RenderPass* renderPass;
        Image** attachments;
        uint32_t attachmentCount;
    };

    DUK_NO_DISCARD virtual ExpectedFrameBuffer create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) = 0;

};

}

#endif //DUK_RHI_RENDERER_H
