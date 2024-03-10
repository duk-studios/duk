/// 19/08/2023
/// renderer.h

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_log/logger.h>
#include <duk_rhi/rhi.h>
#include <duk_scene/scene.h>

namespace duk {

namespace renderer {

class Pass;
class Pipeline;
class GlobalDescriptors;
class ImagePool;
class MaterialPool;
class MeshPool;
class SpritePool;

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    duk::rhi::API api;
    const char* applicationName;
};

class Renderer {
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

    template<typename T, typename... Args>
    T* add_pass(Args&&... args);

private:
    void update_global_descriptors(duk::scene::Objects& objects);

    void update_passes(scene::Objects& objects);

protected:
    duk::platform::Window* m_window;
    std::shared_ptr<duk::rhi::RHI> m_rhi;
    std::shared_ptr<duk::rhi::CommandQueue> m_mainQueue;
    std::vector<std::shared_ptr<Pass>> m_passes;
    std::unique_ptr<GlobalDescriptors> m_globalDescriptors;
};

/// Creates a Renderer with a Forward Pass (and PresentPass if a window is provided)
std::unique_ptr<Renderer> make_forward_renderer(const RendererCreateInfo& rendererCreateInfo);

template<typename T, typename... Args>
T* Renderer::add_pass(Args&&... args) {
    auto pass = std::make_shared<T>(std::forward<Args>(args)...);
    m_passes.push_back(pass);
    return pass.get();
}

}// namespace renderer
}// namespace duk

#endif// DUK_RENDERER_RENDERER_H
