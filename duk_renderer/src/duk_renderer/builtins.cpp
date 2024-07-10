//
// Created by Ricardo on 08/07/2024.
//
#include <duk_renderer/builtins.h>

namespace duk::renderer {

Builtins::Builtins(const BuiltinsCreateInfo& builtinsCreateInfo) {
    const auto pools = builtinsCreateInfo.pools;
    const auto rhi = builtinsCreateInfo.rhi;
    const auto commandQueue = builtinsCreateInfo.commandQueue;
    const auto meshBufferPool = builtinsCreateInfo.meshBufferPool;

    {
        ShaderModuleBuiltinsCreateInfo shaderModuleBuiltinsCreateInfo = {};
        shaderModuleBuiltinsCreateInfo.pools = pools;

        m_shaderModules = std::make_unique<ShaderModuleBuiltins>(shaderModuleBuiltinsCreateInfo);
    }
    {
        ShaderPipelineBuiltinsCreateInfo shaderPipelineBuiltinsCreateInfo = {};
        shaderPipelineBuiltinsCreateInfo.pools = pools;
        shaderPipelineBuiltinsCreateInfo.rhi = rhi;
        shaderPipelineBuiltinsCreateInfo.commandQueue = commandQueue;
        shaderPipelineBuiltinsCreateInfo.shaderModuleBuiltins = m_shaderModules.get();

        m_shaderPipelines = std::make_unique<ShaderPipelineBuiltins>(shaderPipelineBuiltinsCreateInfo);
    }
    {
        ImageBuiltinsCreateInfo imageBuiltinsCreateInfo = {};
        imageBuiltinsCreateInfo.pools = pools;
        imageBuiltinsCreateInfo.rhi = rhi;
        imageBuiltinsCreateInfo.commandQueue = commandQueue;

        m_images = std::make_unique<ImageBuiltins>(imageBuiltinsCreateInfo);
    }
    {
        MeshBuiltinsCreateInfo meshBuiltinsCreateInfo = {};
        meshBuiltinsCreateInfo.pools = pools;
        meshBuiltinsCreateInfo.meshBufferPool = meshBufferPool;

        m_meshes = std::make_unique<MeshBuiltins>(meshBuiltinsCreateInfo);
    }
}

ShaderModuleBuiltins* Builtins::shader_modules() const {
    return m_shaderModules.get();
}

ShaderPipelineBuiltins* Builtins::shader_pipelines() const {
    return m_shaderPipelines.get();
}

ImageBuiltins* Builtins::images() const {
    return m_images.get();
}

MeshBuiltins* Builtins::meshes() const {
    return m_meshes.get();
}

}// namespace duk::renderer
