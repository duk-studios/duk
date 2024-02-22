/// 11/10/2023
/// brush.h

#ifndef DUK_RENDERER_BRUSH_H
#define DUK_RENDERER_BRUSH_H

#include <duk_rhi/command/command_buffer.h>

namespace duk::renderer {

class Brush {
public:

    virtual ~Brush();

    virtual void draw(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) = 0;

};

}

#endif // DUK_RENDERER_BRUSH_H

