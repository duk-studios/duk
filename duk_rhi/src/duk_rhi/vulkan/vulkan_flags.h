/// 20/05/2023
/// vulkan_flags.h

#ifndef DUK_RHI_VULKAN_FLAGS_H
#define DUK_RHI_VULKAN_FLAGS_H

#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

template<typename DukFlagsT, typename F>
VkFlags convert_flags(uint32_t mask, F convertBitFunc) {
    VkFlags flags = 0;
    for (int i = 0; i < DukFlagsT::kCount; i++) {
        auto dukFlagBit = static_cast<typename DukFlagsT::Bits>(1 << i);
        if (mask & 1 << i) {
            flags |= convertBitFunc(dukFlagBit);
        }
    }
    return flags;
}

}// namespace duk::rhi

#endif// DUK_RHI_VULKAN_FLAGS_H
