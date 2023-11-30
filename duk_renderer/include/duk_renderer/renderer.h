/// 19/08/2023
/// renderer.h

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_renderer/sort.h>

#include <duk_rhi/rhi.h>
#include <duk_log/logger.h>

namespace duk {

namespace scene {
class Scene;
class Object;
}

namespace renderer {

class Pass;
class Painter;
class GlobalDescriptors;
class ImagePool;
class MaterialPool;
class MeshPool;

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    duk::rhi::API api;
};

class Renderer {
public:

    using RenderStartEvent = duk::events::EventVoid;

public:

    explicit Renderer(const RendererCreateInfo& rendererCreateInfo);

    virtual ~Renderer();

    void render(duk::scene::Scene* scene);

    DUK_NO_DISCARD uint32_t render_width() const;

    DUK_NO_DISCARD uint32_t render_height() const;

    DUK_NO_DISCARD std::shared_ptr<duk::rhi::Image> create_depth_image(uint32_t width, uint32_t height);

    DUK_NO_DISCARD std::shared_ptr<duk::rhi::Image> create_color_image(uint32_t width, uint32_t height, duk::rhi::PixelFormat format);

    DUK_NO_DISCARD duk::rhi::RHI* rhi() const;

    DUK_NO_DISCARD duk::rhi::CommandQueue* main_command_queue() const;

    DUK_NO_DISCARD RenderStartEvent& render_start_event();

    DUK_NO_DISCARD ImagePool* image_pool();

    DUK_NO_DISCARD MeshPool* mesh_pool();

    DUK_NO_DISCARD MaterialPool* material_pool();

    void use_as_camera(const duk::scene::Object& object);

private:

    void update_global_descriptors(duk::scene::Scene* scene);

protected:
    duk::platform::Window* m_window;
    std::shared_ptr<duk::rhi::RHI> m_rhi;
    std::shared_ptr<duk::rhi::CommandQueue> m_mainQueue;
    std::shared_ptr<duk::rhi::CommandScheduler> m_scheduler;
    std::vector<std::shared_ptr<Pass>> m_passes;
    std::unique_ptr<GlobalDescriptors> m_globalDescriptors;
    std::unique_ptr<ImagePool> m_imagePool;
    std::unique_ptr<MeshPool> m_meshPool;
    std::unique_ptr<MaterialPool> m_materialPool;
    duk::scene::Object::Id m_mainCameraObjectId;
    duk::events::EventVoid m_renderStart;
};

} // namespace renderer
} // namespace duk

#endif // DUK_RENDERER_RENDERER_H
