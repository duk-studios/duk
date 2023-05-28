/// 21/04/2023
/// command_interface.h

#ifndef DUK_RENDERER_COMMAND_INTERFACE_H
#define DUK_RENDERER_COMMAND_INTERFACE_H

#include <duk_renderer/renderer_error.h>
#include <duk_renderer/command/command.h>

#include <duk_macros/macros.h>

#include <tl/expected.hpp>

#include <memory>

namespace duk::renderer {

class RenderPass;
class FrameBuffer;
class Mesh;
class MeshDataSource;
class Pipeline;
class PipelineDataSource;
class DescriptorSet;
class DescriptorSetDataSource;

class CommandBuffer : public Command {
public:

    ~CommandBuffer() override = default;

    virtual void begin() = 0;

    virtual void end() = 0;

    struct RenderPassBeginInfo {
        RenderPass* renderPass;
        FrameBuffer* frameBuffer;
    };

    virtual void begin_render_pass(const RenderPassBeginInfo& renderPassBeginInfo) = 0;

    virtual void end_render_pass() = 0;

    virtual void bind_pipeline(Pipeline* pipeline) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) = 0;

};

}

#endif // DUK_RENDERER_COMMAND_INTERFACE_H

