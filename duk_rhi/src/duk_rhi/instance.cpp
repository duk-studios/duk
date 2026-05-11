//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/instance.h>
#include <duk_rhi/exception.h>
#include <duk_rhi/vulkan/vulkan_instance.h>

namespace duk::rhi {

static std::shared_ptr<VulkanInstance> create_vulkan_rhi(const InstanceCreateInfo& instanceCreateInfo) {
    VulkanRHICreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.hasValidationLayers = instanceCreateInfo.validationLayers;
    vulkanCreateInfo.rhiCreateInfo = instanceCreateInfo;
    return std::make_shared<VulkanInstance>(vulkanCreateInfo);
}

std::shared_ptr<Instance> Instance::create(const InstanceCreateInfo& instanceCreateInfo) {
    switch (instanceCreateInfo.api) {
        case API::UNDEFINED:
            throw Exception(Exception::INVALID_ARGUMENT, "Undefined RHI API");
        case API::VULKAN:
            return create_vulkan_rhi(instanceCreateInfo);
        case API::OPENGL45:
        case API::DX12:
            throw Exception(Exception::NOT_IMPLEMENTED, "RHI API not implemented!");
    }

    throw Exception(Exception::INTERNAL_ERROR, "Unknown error occurred");
}

}// namespace duk::rhi
