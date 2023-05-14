//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_H
#define DUK_RENDERER_H

namespace duk {

struct RendererCreateInfo {

};

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;

private:

};

}

#endif //DUK_RENDERER_H
