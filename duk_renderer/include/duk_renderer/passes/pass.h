/// 05/10/2023
/// pass.h

#ifndef DUK_RENDERER_PASS_H
#define DUK_RENDERER_PASS_H

#include <duk_macros/macros.h>

#include <list>
#include <cstdint>

namespace duk {

namespace rhi {
class CommandBuffer;
class Image;
}
namespace scene {
class Scene;
}

namespace renderer {

class PassConnection {
public:

    PassConnection();

    ~PassConnection();

    void update(duk::rhi::Image* image);

    void connect(PassConnection* connection);

    void disconnect(PassConnection* connection);

    DUK_NO_DISCARD duk::rhi::Image* image() const;

private:
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

