/// 20/05/2023
/// vulkan_flags.h

#ifndef DUK_RENDERER_VULKAN_FLAGS_H
#define DUK_RENDERER_VULKAN_FLAGS_H

#include <duk_renderer/vulkan/vulkan_import.h>

namespace duk::renderer {

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

}

#endif // DUK_RENDERER_VULKAN_FLAGS_H

