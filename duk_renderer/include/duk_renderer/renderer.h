/// 19/08/2023
/// renderer.h

#ifndef DUK_RENDERER_RENDERER_H
#define DUK_RENDERER_RENDERER_H

#include <duk_log/logger.h>
#include <duk_objects/objects.h>
#include <duk_rhi/rhi.h>

namespace duk {

namespace renderer {

class Pass;
class PipelineCache;
class SpriteCache;
class TextCache;
class GlobalDescriptors;
class MeshBufferPool;

struct RendererCreateInfo {
    duk::platform::Window* window;
    duk::log::Logger* logger;
    duk::rhi::API api;
    bool apiValidationLayers;
    const char* applicationName;
};

class Renderer {
public:
    explicit Renderer(const RendererCreateInfo& rendererCreateInfo);

    virtual ~Renderer();

    void render(duk::objects::Objects& objects);

    void clear_cache();

    DUK_NO_DISCARD uint32_t render_width() const;

    DUK_NO_DISCARD uint32_t render_height() const;

    DUK_NO_DISCARD duk::rhi::RHI* rhi() const;

    DUK_NO_DISCARD duk::rhi::CommandQueue* main_command_queue() const;

    DUK_NO_DISCARD GlobalDescriptors* global_descriptors() const;

    DUK_NO_DISCARD MeshBufferPool* mesh_buffer_pool() const;

    DUK_NO_DISCARD SpriteCache* sprite_cache() const;

    DUK_NO_DISCARD TextCache* text_cache() const;

    template<typename T, typename... Args>
    T* add_pass(Args&&... args);

private:
    void update_global_descriptors(duk::objects::Objects& objects);

    void update_passes(objects::Objects& objects);

protected:
    duk::platform::Window* m_window;
    std::shared_ptr<duk::rhi::RHI> m_rhi;
    std::shared_ptr<duk::rhi::CommandQueue> m_mainQueue;
    std::vector<std::shared_ptr<Pass>> m_passes;
    std::unique_ptr<GlobalDescriptors> m_globalDescriptors;
    std::unique_ptr<MeshBufferPool> m_meshBufferPool;
    std::unique_ptr<PipelineCache> m_pipelineCache;
    std::unique_ptr<SpriteCache> m_spriteCache;
    std::unique_ptr<TextCache> m_textMeshCache;
};

template<typename T, typename... Args>
T* Renderer::add_pass(Args&&... args) {
    auto pass = std::make_shared<T>(std::forward<Args>(args)...);
    m_passes.push_back(pass);
    return pass.get();
}

}// namespace renderer
}// namespace duk

#endif// DUK_RENDERER_RENDERER_H
