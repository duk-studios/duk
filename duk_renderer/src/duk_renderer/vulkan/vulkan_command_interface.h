/// 21/04/2023
/// vulkan_command_interface.h

#ifndef DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H
#define DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

#include <duk_renderer/command_interface.h>

namespace duk::renderer {

class VulkanCommandInterface : public CommandInterface {
public:
    ExpectedMesh create_mesh(const MeshDataSource* meshDataSource) override;

    ExpectedPipeline create_pipeline(const PipelineDataSource* pipelineDataSource) override;

    void render(const RenderMeshInfo& renderMeshInfo) override;

private:

};

}

#endif // DUK_RENDERER_VULKAN_COMMAND_INTERFACE_H

