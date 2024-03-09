/// 17/05/2023
/// vulkan_pipeline.h

#ifndef DUK_RHI_VULKAN_PIPELINE_H
#define DUK_RHI_VULKAN_PIPELINE_H

#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::rhi {

VkCullModeFlagBits convert_cull_mode(GraphicsPipeline::CullMode::Bits cullModeBit);

VkCullModeFlags convert_cull_mode_mask(GraphicsPipeline::CullMode::Mask cullModeMask);

VkBlendOp convert_blend_op(GraphicsPipeline::Blend::Operator blendOperator);

VkBlendFactor convert_blend_factor(GraphicsPipeline::Blend::Factor blendFactor);

VkPrimitiveTopology convert_topology(GraphicsPipeline::Topology topology);

VkPolygonMode convert_fill_mode(GraphicsPipeline::FillMode fillMode);

class VulkanShader;
class VulkanRenderPass;
class VulkanResourceManager;

struct VulkanGraphicsPipelineCreateInfo {
    VkDevice device;
    uint32_t imageCount;
    VulkanShader* shader;
    VulkanRenderPass* renderPass;
    VulkanResourceManager* resourceManager;
    GraphicsPipeline::Viewport viewport;
    GraphicsPipeline::Scissor scissor;
    GraphicsPipeline::CullMode::Mask cullModeMask;
    GraphicsPipeline::Blend blend;
    GraphicsPipeline::Topology topology;
    GraphicsPipeline::FillMode fillMode;
    bool depthTesting;
};

class VulkanGraphicsPipeline : public GraphicsPipeline {
public:
    explicit VulkanGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo& pipelineCreateInfo);

    ~VulkanGraphicsPipeline() override;

    void create(uint32_t imageCount);

    void clean(uint32_t imageIndex);

    void clean();

    void update(uint32_t imageIndex);

    DUK_NO_DISCARD const VkPipeline& handle(uint32_t imageIndex) const;

    DUK_NO_DISCARD VkPipelineLayout pipeline_layout() const;

    // Pipeline overrides
    void set_viewport(const Viewport& viewport) override;

    DUK_NO_DISCARD const Viewport& viewport() const override;

    void set_scissor(const Scissor& scissor) override;

    DUK_NO_DISCARD const Scissor& scissor() const override;

    void set_blend(const Blend& blend) override;

    DUK_NO_DISCARD const Blend& blend() const override;

    void set_cull_mode(CullMode::Mask cullModeMask) override;

    DUK_NO_DISCARD CullMode::Mask cull_mode() override;

    void set_topology(Topology topology) override;

    DUK_NO_DISCARD Topology topology() const override;

    void set_fill_mode(FillMode fillMode) override;

    DUK_NO_DISCARD FillMode fill_mode() const override;

    void flush() override;

private:
    VkDevice m_device;
    VulkanShader* m_shader;
    VulkanRenderPass* m_renderPass;
    VulkanResourceManager* m_resourceManager;
    GraphicsPipeline::Viewport m_viewport;
    GraphicsPipeline::Scissor m_scissor;
    GraphicsPipeline::CullMode::Mask m_cullModeMask;
    GraphicsPipeline::Blend m_blend;
    GraphicsPipeline::Topology m_topology;
    GraphicsPipeline::FillMode m_fillMode;
    bool m_depthTesting;
    std::vector<VkPipeline> m_pipelines;
};

}// namespace duk::rhi
#endif// DUK_RHI_VULKAN_PIPELINE_H
