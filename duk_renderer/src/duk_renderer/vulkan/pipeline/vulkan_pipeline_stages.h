/// 16/06/2023
/// vulkan_pipeline_stages.h

#ifndef DUK_RENDERER_VULKAN_PIPELINE_STAGES_H
#define DUK_RENDERER_VULKAN_PIPELINE_STAGES_H

#include <duk_renderer/pipeline/pipeline_stages.h>
#include <duk_renderer/vulkan/vulkan_import.h>

namespace duk::renderer {

VkPipelineStageFlagBits convert_pipeline_stage(PipelineStage::Bits pipelineStage);

VkPipelineStageFlags convert_pipeline_stage_mask(PipelineStage::Mask pipelineStageMask);

}

#endif // DUK_RENDERER_VULKAN_PIPELINE_STAGES_H

