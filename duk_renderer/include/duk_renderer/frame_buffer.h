/// 23/04/2023
/// frame_buffer.h

#ifndef DUK_RENDERER_FRAME_BUFFER_H
#define DUK_RENDERER_FRAME_BUFFER_H

#include <duk_macros/macros.h>

#include <cstdint>

namespace duk::renderer {

class FrameBuffer {
public:

    virtual ~FrameBuffer();

    DUK_NO_DISCARD virtual uint32_t width() const = 0;

    DUK_NO_DISCARD virtual uint32_t height() const = 0;

};

}

#endif // DUK_RENDERER_FRAME_BUFFER_H

