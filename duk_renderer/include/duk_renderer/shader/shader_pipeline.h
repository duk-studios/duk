//
// Created by Ricardo on 03/05/2024.
//

#ifndef DUK_RENDERER_SHADER_PIPELINE_H
#define DUK_RENDERER_SHADER_PIPELINE_H

#include <duk_renderer/shader/pipeline_cache.h>
#include <duk_renderer/shader/shader_pipeline_data.h>

#include <duk_resource/handle.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <duk_rhi/command/command_buffer.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class Renderer;
class PipelineCache;

struct ShaderPipelineCreateInfo {
    duk::rhi::RHI* rhi;
    const ShaderPipelineData* shaderPipelineData;
};

class ShaderPipeline {
public:
    explicit ShaderPipeline(const ShaderPipelineCreateInfo& shaderPipelineCreateInfo);

    const duk::rhi::DescriptorSetDescription& descriptor_set_description() const;

    void set_blend(bool enable);

    bool blend() const;

    void set_depth(bool enable);

    bool depth() const;

    void set_invert_y(bool invert);

    bool invert_y() const;

    uint32_t priority() const;

    void update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport);

    void bind(duk::rhi::CommandBuffer* commandBuffer) const;

    // hack: we need to create the shader object after solving our references,
    // that's why we specialize both methods.
    // we should be able to remove this if we change how we load resources, starting with the leafs instead of the root
    void solve_resources(duk::resource::DependencySolver* solver);

    void solve_resources(duk::resource::ReferenceSolver* solver);

private:
    void create_shader();

private:
    duk::rhi::RHI* m_rhi;
    std::unordered_map<duk::rhi::ShaderModule::Bits, ShaderModuleResource> m_shaderModules;
    std::shared_ptr<duk::rhi::Shader> m_shader;
    std::shared_ptr<duk::rhi::GraphicsPipeline> m_pipeline;
    PipelineState m_state;
    uint32_t m_priority;
    bool m_invertY;
    bool m_dirty;
};

using ShaderPipelineResource = duk::resource::Handle<ShaderPipeline>;

}// namespace duk::renderer

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::ShaderPipeline& shaderPipeline) {
    shaderPipeline.solve_resources(solver);
}

}// namespace duk::resource

#endif//DUK_RENDERER_SHADER_PIPELINE_H
