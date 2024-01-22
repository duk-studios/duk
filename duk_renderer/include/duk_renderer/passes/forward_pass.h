/// 05/10/2023
/// forward_pass.h

#ifndef DUK_RENDERER_FORWARD_PASS_H
#define DUK_RENDERER_FORWARD_PASS_H

#include <duk_renderer/passes/pass.h>
#include <duk_renderer/resources/materials/draw_entry.h>
#include <duk_renderer/brushes/sprite_brush.h>

#include <duk_tools/fixed_vector.h>

namespace duk::renderer {

class Renderer;

struct DrawData {
    duk::tools::FixedVector<ObjectEntry, scene::MAX_OBJECTS> objects;
    duk::tools::FixedVector<uint16_t, scene::MAX_OBJECTS> sortedObjects;

    void clear();
};

struct MeshDrawData {
    duk::tools::FixedVector<MeshDrawEntry, scene::MAX_OBJECTS> drawEntries;

    void clear();
};

struct SpriteDrawData {
    std::unique_ptr<SpriteBrushPool> spriteBrushPool;
};

struct ForwardPassCreateInfo {
    Renderer* renderer;
};

class ForwardPass : public Pass {
public:

    explicit ForwardPass(const ForwardPassCreateInfo& forwardPassCreateInfo);

    ~ForwardPass() override;

    void render(const RenderParams& renderParams) override;

    DUK_NO_DISCARD PassConnection* out_color();

private:
    Renderer* m_renderer;
    std::shared_ptr<duk::rhi::Image> m_colorImage;
    std::shared_ptr<duk::rhi::Image> m_depthImage;
    std::shared_ptr<duk::rhi::RenderPass> m_renderPass;
    std::shared_ptr<duk::rhi::FrameBuffer> m_frameBuffer;
    PassConnection m_outColor;
    DrawData m_drawData;
    MeshDrawData m_meshDrawData;
};

}

#endif // DUK_RENDERER_FORWARD_PASS_H

