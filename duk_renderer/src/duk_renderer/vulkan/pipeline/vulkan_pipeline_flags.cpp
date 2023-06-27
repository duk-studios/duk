/// 16/06/2023
/// vulkan_pipeline_stages.cpp

#include <duk_renderer/vulkan/pipeline/vulkan_pipeline_flags.h>
#include <duk_renderer/vulkan/vulkan_flags.h>

#include <stdexcept>

namespace duk::renderer {

VkPipelineStageFlagBits convert_pipeline_stage(PipelineStage::Bits pipelineStage) {
    VkPipelineStageFlagBits converted;
    switch (pipelineStage) {
        case PipelineStage::NONE: converted = VK_PIPELINE_STAGE_NONE; break;
        case PipelineStage::TOP_OF_PIPE: converted = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; break;
        case PipelineStage::DRAW_INDIRECT: converted = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT; break;
        case PipelineStage::VERTEX_INPUT: converted = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; break;
        case PipelineStage::VERTEX_SHADER: converted = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; break;
        case PipelineStage::TESSELLATION_CONTROL_SHADER: converted = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; break;
        case PipelineStage::TESSELLATION_EVALUATION_SHADER: converted = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; break;
        case PipelineStage::GEOMETRY_SHADER: converted = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; break;
        case PipelineStage::FRAGMENT_SHADER: converted = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
        case PipelineStage::EARLY_FRAGMENT_TESTS: converted = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; break;
        case PipelineStage::LATE_FRAGMENT_TESTS: converted = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; break;
        case PipelineStage::COLOR_ATTACHMENT_OUTPUT: converted = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; break;
        case PipelineStage::COMPUTE_SHADER: converted = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
        case PipelineStage::TRANSFER: converted = VK_PIPELINE_STAGE_TRANSFER_BIT; break;
        case PipelineStage::BOTTOM_OF_PIPE: converted = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; break;
        case PipelineStage::HOST: converted = VK_PIPELINE_STAGE_HOST_BIT; break;
        case PipelineStage::ALL_GRAPHICS: converted = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; break;
        case PipelineStage::ALL_COMMANDS: converted = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; break;
        default:
            throw std::invalid_argument("unhandled PipelineStage::Bits for vulkan");
    }
    return converted;
}

VkPipelineStageFlags convert_pipeline_stage_mask(PipelineStage::Mask pipelineStageMask) {
    return convert_flags<PipelineStage>(pipelineStageMask, convert_pipeline_stage);
}

VkAccessFlagBits convert_access(Access::Bits access) {
    VkAccessFlagBits converted;
    switch (access) {
        case Access::NONE: converted = VK_ACCESS_NONE; break;
        case Access::INDIRECT_COMMAND_READ: converted = VK_ACCESS_INDIRECT_COMMAND_READ_BIT; break;
        case Access::INDEX_READ: converted = VK_ACCESS_INDEX_READ_BIT; break;
        case Access::VERTEX_ATTRIBUTE_READ: converted = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT; break;
        case Access::UNIFORM_READ: converted = VK_ACCESS_UNIFORM_READ_BIT; break;
        case Access::INPUT_ATTACHMENT_READ: converted = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; break;
        case Access::SHADER_READ: converted = VK_ACCESS_SHADER_READ_BIT; break;
        case Access::SHADER_WRITE: converted = VK_ACCESS_SHADER_WRITE_BIT; break;
        case Access::COLOR_ATTACHMENT_READ: converted = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT; break;
        case Access::COLOR_ATTACHMENT_WRITE: converted = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case Access::DEPTH_STENCIL_ATTACHMENT_READ: converted = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT; break;
        case Access::DEPTH_STENCIL_ATTACHMENT_WRITE: converted = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case Access::TRANSFER_READ: converted = VK_ACCESS_TRANSFER_READ_BIT; break;
        case Access::TRANSFER_WRITE: converted = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case Access::HOST_READ: converted = VK_ACCESS_HOST_READ_BIT; break;
        case Access::HOST_WRITE: converted = VK_ACCESS_HOST_WRITE_BIT; break;
        case Access::MEMORY_READ: converted = VK_ACCESS_MEMORY_READ_BIT; break;
        case Access::MEMORY_WRITE: converted = VK_ACCESS_MEMORY_WRITE_BIT; break;
        default:
            throw std::invalid_argument("unhandled Access::Bits for vulkan");
    }
    return converted;
}

VkAccessFlags convert_access_mask(Access::Mask accessMask) {
    return convert_flags<Access>(accessMask, convert_access);
}

}