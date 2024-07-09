//
// Created by Ricardo on 08/07/2024.
//

#ifndef DUK_RENDERER_BUILTINS_H
#define DUK_RENDERER_BUILTINS_H

#include <duk_renderer/shader/shader_module_builtins.h>
#include <duk_renderer/shader/shader_pipeline_builtins.h>
#include <duk_renderer/image/image_builtins.h>
#include <duk_renderer/mesh/mesh_builtins.h>

namespace duk::renderer {

struct BuiltinsCreateInfo {
    duk::resource::Pools* pools;
    duk::rhi::RHI* rhi;
    duk::rhi::CommandQueue* commandQueue;
    MeshBufferPool* meshBufferPool;
};

class Builtins {
public:
    Builtins(const BuiltinsCreateInfo& builtinsCreateInfo);

    ShaderModuleBuiltins* shader_modules() const;

    ShaderPipelineBuiltins* shader_pipelines() const;

    ImageBuiltins* images() const;

    MeshBuiltins* meshes() const;

private:
    std::unique_ptr<ShaderModuleBuiltins> m_shaderModules;
    std::unique_ptr<ShaderPipelineBuiltins> m_shaderPipelines;
    std::unique_ptr<ImageBuiltins> m_images;
    std::unique_ptr<MeshBuiltins> m_meshes;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_BUILTINS_H
