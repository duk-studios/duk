/// 19/08/2023
/// renderer.h

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_renderer/sort.h>
#include <duk_renderer/painters/painter.h>

#include <duk_rhi/rhi.h>
#include <duk_log/logger.h>

namespace duk::scene {
class Scene;
}

namespace duk::renderer {

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    duk::rhi::API api;
};

class Renderer {
public:
    struct ObjectEntry {
        duk::scene::Object::Id objectId;
        Mesh* mesh {};
        Painter* painter {};
        Palette* palette {};
        SortKey sortKey {};
    };

    struct PaintEntry {
        Painter::PaintParams params;
        Painter* painter;
    };

public:

    explicit Renderer(const RendererCreateInfo& rendererCreateInfo);

    virtual void render(duk::scene::Scene* scene) = 0;

    virtual void resize(uint32_t width, uint32_t height);

    DUK_NO_DISCARD uint32_t render_width() const;

    DUK_NO_DISCARD uint32_t render_height() const;

    DUK_NO_DISCARD std::shared_ptr<duk::rhi::Image> create_depth_image(uint32_t width, uint32_t height);

    DUK_NO_DISCARD duk::rhi::RHI* rhi() const;

    DUK_NO_DISCARD duk::rhi::CommandQueue* main_command_queue() const;

protected:
    duk::platform::Window* m_window;
    std::shared_ptr<duk::rhi::RHI> m_rhi;
    std::shared_ptr<duk::rhi::CommandQueue> m_mainQueue;
    std::shared_ptr<duk::rhi::CommandScheduler> m_scheduler;
};

// specialization for duk_renderer/sort.h sort_key
template<>
inline SortKey SortKey::sort_key(const Renderer::ObjectEntry& objectEntry) {
    return objectEntry.sortKey;
}

}

#endif // DUK_RENDERER_RENDERER_H

