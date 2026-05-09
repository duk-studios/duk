/// 17/05/2023
/// vulkan_pipeline.h

#ifndef DUK_RHI_VULKAN_PIPELINE_H
#define DUK_RHI_VULKAN_PIPELINE_H

#include <duk_rhi/pipeline/graphics_pipeline.h>
#include <duk_rhi/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

namespace duk::rhi {

VkCullModeFlagBits convert_cull_mode(GraphicsPipeline::CullMode::Bits cullModeBit);

VkCullModeFlags convert_cull_mode_mask(GraphicsPipeline::CullMode::Mask cullModeMask);

VkBlendOp convert_blend_op(GraphicsPipeline::Blend::Operator blendOperator);

VkBlendFactor convert_blend_factor(GraphicsPipeline::Blend::Factor blendFactor);

VkPrimitiveTopology convert_topology(GraphicsPipeline::Topology topology);

VkPolygonMode convert_fill_mode(GraphicsPipeline::FillMode fillMode);

class VulkanShader;
class VulkanRenderPass;

struct VulkanGraphicsPipelineCreateInfo {
    VkDevice device;
    VulkanShader* shader;
    VulkanRenderPass* renderPass;
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

    /// Returns the single VkPipeline handle.
    DUK_NO_DISCARD VkPipeline handle() const;

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

    /// Immediately destroys and recreates the VkPipeline with current settings.
    /// Caller must ensure the GPU is not using the pipeline before calling.
    void flush() override;

private:
    void recreate();

    void destroy();

private:
    VkDevice m_device;
    VulkanShader* m_shader;
    VulkanRenderPass* m_renderPass;
    Viewport m_viewport;
    Scissor m_scissor;
    CullMode::Mask m_cullModeMask;
    Blend m_blend;
    Topology m_topology;
    FillMode m_fillMode;
    bool m_depthTesting;
    VkPipeline m_pipeline{VK_NULL_HANDLE};
};

}// namespace duk::rhi
#endif// DUK_RHI_VULKAN_PIPELINE_H
