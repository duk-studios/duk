/// 12/09/2023
/// palette.h

#ifndef DUK_RENDERER_PALETTE_H
#define DUK_RENDERER_PALETTE_H

#include <duk_rhi/command/command_buffer.h>
#include <duk_scene/object.h>

namespace duk::renderer {

class Palette {
public:

    struct UpdateParams {
        duk::scene::Object object;
    };

    virtual void update(const UpdateParams& params) = 0;

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer) = 0;

};

}

#endif // DUK_RENDERER_PALETTE_H

