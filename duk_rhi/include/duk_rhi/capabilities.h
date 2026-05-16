/// 08/06/2023
/// capabilities.h

#ifndef DUK_RHI_CAPABILITIES_H
#define DUK_RHI_CAPABILITIES_H

#include <cstddef>

namespace duk::rhi {

struct Capabilities {
    /// True when the device supports multi-draw-indirect commands.
    bool multiDrawIndirectSupported;

    /// Minimum required alignment (in bytes) for the offset of a uniform buffer binding.
    size_t minUniformBufferOffsetAlignment;

    /// Minimum required alignment (in bytes) for the offset of a storage buffer binding.
    size_t minStorageBufferOffsetAlignment;
};

}// namespace duk::rhi

#endif// DUK_RHI_CAPABILITIES_H
