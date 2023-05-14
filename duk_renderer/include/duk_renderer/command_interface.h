/// 21/04/2023
/// command_interface.h

#ifndef DUK_RENDERER_COMMAND_INTERFACE_H
#define DUK_RENDERER_COMMAND_INTERFACE_H

#include <duk_renderer/renderer_error.h>

#include <duk_macros/macros.h>

#include <tl/expected.hpp>

#include <memory>

namespace duk::renderer {

class Mesh;
class MeshDataSource;
class Pipeline;
class PipelineDataSource;
class DescriptorSet;
class DescriptorSetDataSource;

using ExpectedMesh = tl::expected<std::shared_ptr<Mesh>, RendererError>;
using ExpectedPipeline = tl::expected<std::shared_ptr<Pipeline>, RendererError>;

struct RenderMeshInfo {
    Mesh* mesh;
    Pipeline* pipeline;
    DescriptorSet* descriptorSet;
};

class CommandInterface {
public:

    DUK_NO_DISCARD virtual ExpectedMesh create_mesh(const MeshDataSource* meshDataSource) = 0;

    DUK_NO_DISCARD virtual ExpectedPipeline create_pipeline(const PipelineDataSource* pipelineDataSource) = 0;

    virtual void render(const RenderMeshInfo& renderMeshInfo) = 0;

};

}

#endif // DUK_RENDERER_COMMAND_INTERFACE_H

