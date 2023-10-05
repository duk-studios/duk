/// 05/10/2023
/// pass.h

#ifndef DUK_RENDERER_PASS_H
#define DUK_RENDERER_PASS_H

#include <cstdint>

namespace duk {

namespace rhi {
class CommandBuffer;
}
namespace scene {
class Scene;
}

namespace renderer {

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

