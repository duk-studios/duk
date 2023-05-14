/// 21/04/2023
/// vulkan_command_interface.cpp

#include <duk_renderer/vulkan/vulkan_command_interface.h>

namespace duk::renderer {

ExpectedMesh VulkanCommandInterface::create_mesh(const MeshDataSource* meshDataSource) {
    return ExpectedMesh();
}

ExpectedPipeline VulkanCommandInterface::create_pipeline(const PipelineDataSource* pipelineDataSource) {
    return ExpectedPipeline();
}

void VulkanCommandInterface::render(const RenderMeshInfo& renderMeshInfo) {

}

}
