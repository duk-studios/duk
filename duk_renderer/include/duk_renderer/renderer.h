//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_renderer/renderer_error.h>
#include <duk_renderer/command/command_queue.h>
#include <duk_renderer/command/command_scheduler.h>
#include <duk_renderer/render_pass.h>
#include <duk_renderer/buffer.h>
#include <duk_renderer/image.h>
#include <duk_renderer/image_data_source.h>
#include <duk_renderer/descriptor_set.h>
#include <duk_renderer/frame_buffer.h>
#include <duk_renderer/mesh/mesh.h>
#include <duk_renderer/pipeline/shader_data_source.h>
#include <duk_renderer/pipeline/shader.h>
#include <duk_renderer/pipeline/pipeline.h>

#include <duk_log/logger.h>

#include <tl/expected.hpp>

#include <memory>
#include <optional>

namespace duk::platform {

class Window;

}

namespace duk::renderer {

enum class RendererAPI {
    UNDEFINED,
    VULKAN,
    OPENGL45,
    DX12
};

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    const char* applicationName;
    uint32_t applicationVersion;
    const char* engineName;
    uint32_t engineVersion;
    RendererAPI api;
    size_t deviceIndex;
};

class Renderer;
using ExpectedRenderer = tl::expected<std::shared_ptr<Renderer>, RendererError>;
using ExpectedCommandQueue = tl::expected<std::shared_ptr<CommandQueue>, RendererError>;
using ExpectedMesh = tl::expected<std::shared_ptr<Mesh>, RendererError>;
using ExpectedShader = tl::expected<std::shared_ptr<Shader>, RendererError>;
using ExpectedPipeline = tl::expected<std::shared_ptr<Pipeline>, RendererError>;
using ExpectedRenderPass = tl::expected<std::shared_ptr<RenderPass>, RendererError>;
using ExpectedFrameBuffer = tl::expected<std::shared_ptr<FrameBuffer>, RendererError>;
using ExpectedBuffer = tl::expected<std::shared_ptr<Buffer>, RendererError>;
using ExpectedImage = tl::expected<std::shared_ptr<Image>, RendererError>;
using ExpectedDescriptorSet = tl::expected<std::shared_ptr<DescriptorSet>, RendererError>;
using ExpectedCommandScheduler = tl::expected<std::shared_ptr<CommandScheduler>, RendererError>;

class Renderer {
public:

    static ExpectedRenderer create_renderer(const RendererCreateInfo& rendererCreateInfo);

    /// destructor
    virtual ~Renderer() = default;

    /// called to start a new frame
    virtual void prepare_frame() = 0;

    /// returns a command that acquires the next image for presentation
    virtual Command* acquire_image_command() = 0;

    /// returns a command that presents the contents of the present image on the screen
    virtual Command* present_command() = 0;

    /// returns the current present image
    virtual Image* present_image() = 0;

    struct CommandQueueCreateInfo {
        CommandQueueType type;
    };

    DUK_NO_DISCARD virtual ExpectedCommandQueue create_command_queue(const CommandQueueCreateInfo& commandQueueCreateInfo) = 0;

    struct MeshCreateInfo {
        MeshDataSource* meshDataSource;
        CommandQueueType ownerQueueType;
    };

    DUK_NO_DISCARD virtual ExpectedCommandScheduler create_command_scheduler() = 0;

    DUK_NO_DISCARD virtual ExpectedMesh create_mesh(const MeshCreateInfo& meshCreateInfo) = 0;

    struct ShaderCreateInfo {
        ShaderDataSource* shaderDataSource;
    };

    DUK_NO_DISCARD virtual ExpectedShader create_shader(const ShaderCreateInfo& shaderCreateInfo) = 0;

    struct PipelineCreateInfo {
        Shader* shader;
        RenderPass* renderPass;
        Pipeline::Viewport viewport;
        Pipeline::Scissor scissor;
        Pipeline::CullMode::Mask cullModeMask;
        Pipeline::Blend blend;
        bool depthTesting;
    };

    DUK_NO_DISCARD virtual ExpectedPipeline create_pipeline(const PipelineCreateInfo& pipelineCreateInfo) = 0;

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
    };

    DUK_NO_DISCARD virtual ExpectedBuffer create_buffer(const BufferCreateInfo& bufferCreateInfo) = 0;

    struct ImageCreateInfo {
        ImageDataSource* imageDataSource;
        Image::Layout initialLayout;
        Image::Usage usage;
        Image::UpdateFrequency updateFrequency;
    };

    DUK_NO_DISCARD virtual ExpectedImage create_image(const ImageCreateInfo& imageCreateInfo) = 0;

    struct DescriptorSetCreateInfo {
        DescriptorSetDescription description;
    };

    DUK_NO_DISCARD virtual ExpectedDescriptorSet create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetCreateInfo) = 0;

    struct FrameBufferCreateInfo {
        RenderPass* renderPass;
        Image* attachments;
        uint32_t attachmentCount;
    };

    DUK_NO_DISCARD virtual ExpectedFrameBuffer create_frame_buffer(const FrameBufferCreateInfo& frameBufferCreateInfo) = 0;

private:

};

}

#endif //DUK_RENDERER_RENDERER_H
