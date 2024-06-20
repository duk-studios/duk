/// 05/10/2023
/// pass.h

#ifndef DUK_RENDERER_PASS_H
#define DUK_RENDERER_PASS_H

#include <duk_macros/macros.h>
#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/pipeline/pipeline_flags.h>

#include <duk_resource/pool.h>

#include <list>

namespace duk {

namespace objects {
class Objects;
}

namespace renderer {

class PipelineCache;
class SpriteCache;
class GlobalDescriptors;

class PassConnection {
public:
    PassConnection(duk::rhi::Access::Mask accessMask, duk::rhi::PipelineStage::Mask stageMask, duk::rhi::Image::Layout imageLayout);

    ~PassConnection();

    void update(duk::rhi::Image* image);

    void connect(PassConnection* connection);

    void disconnect(PassConnection* connection);

    DUK_NO_DISCARD duk::rhi::Image* image() const;

    DUK_NO_DISCARD duk::rhi::Access::Mask access_mask() const;

    DUK_NO_DISCARD duk::rhi::PipelineStage::Mask stage_mask() const;

    DUK_NO_DISCARD duk::rhi::Image::Layout image_layout() const;

    DUK_NO_DISCARD duk::rhi::Access::Mask parent_access_mask() const;

    DUK_NO_DISCARD duk::rhi::PipelineStage::Mask parent_stage_mask() const;

    DUK_NO_DISCARD duk::rhi::Image::Layout parent_image_layout() const;

    DUK_NO_DISCARD duk::rhi::CommandBuffer::ImageMemoryBarrier image_memory_barrier() const;

private:
    duk::rhi::Access::Mask m_accessMask;
    duk::rhi::PipelineStage::Mask m_stageMask;
    duk::rhi::Image::Layout m_imageLayout;
    duk::rhi::Image* m_image;
    PassConnection* m_parent;
    std::list<PassConnection*> m_children;
};

class Pass {
public:
    virtual ~Pass();

    struct UpdateParams {
        duk::objects::Objects* objects;
        GlobalDescriptors* globalDescriptors;
        glm::vec2 viewport;
        PipelineCache* pipelineCache;
        SpriteCache* spriteCache;
        duk::resource::Pools* pools;
    };

    virtual void update(const UpdateParams& params) = 0;

    virtual void render(duk::rhi::CommandBuffer* commandBuffer) = 0;
};

}// namespace renderer
}// namespace duk

#endif// DUK_RENDERER_PASS_H
