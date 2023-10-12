/// 05/10/2023
/// pass.h

#ifndef DUK_RENDERER_PASS_H
#define DUK_RENDERER_PASS_H

#include <duk_macros/macros.h>
#include <duk_rhi/pipeline/pipeline_flags.h>
#include <duk_rhi/command/command_buffer.h>

#include <list>
#include <cstdint>

namespace duk {

namespace scene {
class Scene;
}

namespace renderer {

class PassConnection {
public:

    PassConnection(duk::rhi::Access::Mask accessMask, duk::rhi::PipelineStage::Mask stageMask);

    ~PassConnection();

    void update(duk::rhi::Image* image);

    void connect(PassConnection* connection);

    void disconnect(PassConnection* connection);

    DUK_NO_DISCARD duk::rhi::Image* image() const;

    DUK_NO_DISCARD duk::rhi::Access::Mask access_mask() const;

    DUK_NO_DISCARD duk::rhi::PipelineStage::Mask stage_mask() const;

    DUK_NO_DISCARD duk::rhi::Access::Mask parent_access_mask() const;

    DUK_NO_DISCARD duk::rhi::PipelineStage::Mask parent_stage_mask() const;

    DUK_NO_DISCARD duk::rhi::CommandBuffer::ImageMemoryBarrier image_memory_barrier() const;

private:
    duk::rhi::Access::Mask m_accessMask;
    duk::rhi::PipelineStage::Mask m_stageMask;
    duk::rhi::Image* m_image;
    PassConnection* m_parent;
    std::list<PassConnection*> m_children;
};

class Pass {
public:

    virtual ~Pass();

    struct RenderParams {
        duk::rhi::CommandBuffer* commandBuffer;
        duk::scene::Scene* scene;
        uint32_t outputWidth;
        uint32_t outputHeight;
    };

    virtual void render(const RenderParams& renderParams) = 0;

};

}
}

#endif // DUK_RENDERER_PASS_H

