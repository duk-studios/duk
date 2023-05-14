//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

namespace duk::renderer {

struct RendererCreateInfo {

};

class Renderer {
public:

    /// destructor
    virtual ~Renderer() = default;

    /// called to start a new frame
    virtual void begin_frame() = 0;

    /// called to end a frame, TODO: not sure if it will already present the frame
    virtual void end_frame() = 0;

private:

};

}

#endif //DUK_RENDERER_RENDERER_H
