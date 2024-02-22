/// 12/11/2023
/// pixel.cpp

#include <duk_rhi/pixel.h>

#include <stdexcept>

namespace duk::rhi {

PixelFormat::PixelFormat()
    : PixelFormat(UNDEFINED) {
}

PixelFormat::PixelFormat(uint32_t mask)
    : m_mask(static_cast<PixelFormat::Mask>(mask)) {
}

PixelFormat& PixelFormat::operator=(uint32_t mask) {
    m_mask = static_cast<PixelFormat::Mask>(mask);
    return *this;
}

PixelFormat PixelFormat::operator&(uint32_t mask) const {
    return m_mask & mask;
}

PixelFormat PixelFormat::operator|(uint32_t mask) const {
    return m_mask | mask;
}

PixelFormat::operator uint32_t() const {
    return m_mask;
}

uint32_t PixelFormat::channel_count() const {
    const uint32_t setChannelBit = static_cast<uint32_t>(m_mask) & PixelFlags::CHANNEL_MASK;
    switch (setChannelBit) {
        case PixelFlags::CHANNEL_LAYOUT_R:
            return 1;
        case PixelFlags::CHANNEL_LAYOUT_RG:
            return 2;
        case PixelFlags::CHANNEL_LAYOUT_RGB:
        case PixelFlags::CHANNEL_LAYOUT_BGR:
            return 3;
        case PixelFlags::CHANNEL_LAYOUT_RGBA:
        case PixelFlags::CHANNEL_LAYOUT_BGRA:
            return 4;
        default:
            throw std::logic_error("undefined PixelFormat channel count");
    }
}

uint32_t PixelFormat::mask() const {
    return m_mask;
}

bool PixelFormat::is_depth() const {
    return (static_cast<uint32_t>(m_mask) & PixelFlags::DEPTH_MASK) != 0;
}

bool PixelFormat::is_stencil() const {
    return (static_cast<uint32_t>(m_mask) & PixelFlags::DEPTH_STENCIL) != 0;
}

size_t PixelFormat::channel_size() const {
    const auto channelBit = static_cast<uint32_t>(m_mask) & PixelFlags::BIT_WIDTH_MASK;
    switch (channelBit) {
        case PixelFlags::CHANNEL_BIT_WIDTH_8:
            return 1;
        case PixelFlags::CHANNEL_BIT_WIDTH_16:
            return 2;
        case PixelFlags::CHANNEL_BIT_WIDTH_24:
            return 3;
        case PixelFlags::CHANNEL_BIT_WIDTH_32:
            return 4;
        case PixelFlags::CHANNEL_BIT_WIDTH_40:
            return 5;
        default:
            throw std::runtime_error("unknown channel bit width");
    }
}

size_t PixelFormat::size() const {
    return channel_count() * channel_size();
}

uint32_t operator|(uint32_t mask, PixelFormat pixelFormat) {
    return mask | pixelFormat.mask();
}

uint32_t operator&(uint32_t mask, PixelFormat pixelFormat) {
    return mask & pixelFormat.mask();
}

}// namespace duk::rhi