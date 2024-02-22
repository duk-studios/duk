/// 16/06/2023
/// vulkan_pipeline_stages.h

#ifndef DUK_RHI_VULKAN_PIPELINE_STAGES_H
#define DUK_RHI_VULKAN_PIPELINE_STAGES_H

#include <duk_rhi/pipeline/pipeline_flags.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

VkPipelineStageFlagBits convert_pipeline_stage(PipelineStage::Bits pipelineStage);

VkPipelineStageFlags convert_pipeline_stage_mask(PipelineStage::Mask pipelineStageMask);

VkAccessFlagBits convert_access(Access::Bits access);

VkAccessFlags convert_access_mask(Access::Mask accessMask);

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_PIPELINE_STAGES_H
