//
// Created by rov on 11/05/2026.
//

#include <duk_rhi/vulkan/vulkan_render_state.h>

namespace duk::rhi {

VkAttachmentStoreOp convert_store_op(StoreOp storeOp) {
    VkAttachmentStoreOp converted;
    switch (storeOp) {
        case StoreOp::STORE:
            converted = VK_ATTACHMENT_STORE_OP_STORE;
            break;
        case StoreOp::DONT_CARE:
            converted = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        default:
            throw std::invalid_argument("unhandled StoreOp for Vulkan");
    }
    return converted;
}

VkAttachmentLoadOp convert_load_op(LoadOp loadOp) {
    VkAttachmentLoadOp converted;
    switch (loadOp) {
        case LoadOp::LOAD:
            converted = VK_ATTACHMENT_LOAD_OP_LOAD;
            break;
        case LoadOp::CLEAR:
            converted = VK_ATTACHMENT_LOAD_OP_CLEAR;
            break;
        case LoadOp::DONT_CARE:
            converted = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            break;
        default:
            throw std::invalid_argument("unhandled StoreOp for Vulkan");
    }
    return converted;
}

}// namespace duk::rhi